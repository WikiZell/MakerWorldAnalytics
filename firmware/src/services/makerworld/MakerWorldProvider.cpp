#include "mwa/MakerWorldProvider.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "mwa/ProfileInput.h"

namespace mwa {
namespace {
constexpr uint32_t kRequestTimeoutMs = 10000;
constexpr char kUserAgent[] = "MakerWorldAnalytics/0.1.0-alpha.1 (public-profile-probe)";
// GTS Root R4; verified against MakerWorld's current WE1 chain on 2026-07-21.
constexpr char kGoogleTrustServicesRootR4[] = R"CERT(-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----)CERT";

bool parseCompactCount(String value, uint32_t& result) {
  value.trim();
  value.toLowerCase();
  value.replace(",", "");
  value.replace(" ", "");
  float multiplier = 1.0F;
  if (value.endsWith("k")) {
    multiplier = 1000.0F;
    value.remove(value.length() - 1);
  }
  if (value.isEmpty()) return false;
  for (const char character : value) {
    if ((character < '0' || character > '9') && character != '.') return false;
  }
  result = static_cast<uint32_t>(value.toFloat() * multiplier + 0.5F);
  return true;
}

void parseProfileFields(Stream& body, ProfileProbe& result) {
  String token;
  String previous;
  token.reserve(96);
  bool inTag = false;
  const uint32_t deadline = millis() + kRequestTimeoutMs;
  auto completeToken = [&]() {
    token.trim();
    if (token.isEmpty()) return;
    uint32_t value = 0;
    if (token == "Followers" && parseCompactCount(previous, value)) {
      result.followers = value;
      result.hasFollowerCount = true;
    } else if (token == "Following" && parseCompactCount(previous, value)) {
      result.following = value;
      result.hasFollowingCount = true;
    } else if (token.startsWith("3D Models (")) {
      const int begin = token.indexOf('(');
      const int end = token.indexOf(')', begin + 1);
      if (begin >= 0 && end > begin && parseCompactCount(token.substring(begin + 1, end), value)) {
        result.models = value;
        result.hasModelCount = true;
      }
    }
    previous = token;
    token = "";
  };
  while (millis() < deadline && (!result.hasFollowerCount || !result.hasFollowingCount || !result.hasModelCount)) {
    if (body.available() == 0) {
      delay(1);
      continue;
    }
    const char character = static_cast<char>(body.read());
    if (character == '<') {
      completeToken();
      inTag = true;
    } else if (character == '>') {
      inTag = false;
    } else if (!inTag) {
      if (token.length() < 96) token += character;
    }
  }
  completeToken();
}

ProfileProbe failure(ProfileProbeState state, uint16_t status, const String& url, const String& message) {
  ProfileProbe result;
  result.state = state;
  result.httpStatus = status;
  result.profileUrl = url;
  result.message = message;
  return result;
}
}  // namespace

String MakerWorldProvider::publicProfileUrl(const String& username) {
  return "https://makerworld.com/en/@" + username;
}

ProfileProbe MakerWorldProvider::probePublicProfile(const String& usernameOrUrl) const {
  const ProfileInputResult parsed = normalizeProfileInput(usernameOrUrl.c_str());
  if (!parsed.valid) return failure(ProfileProbeState::InvalidInput, 0, "", parsed.error.c_str());
  const String url = publicProfileUrl(parsed.username.c_str());
  if (WiFi.status() != WL_CONNECTED) return failure(ProfileProbeState::NetworkUnavailable, 0, url, "Wi-Fi is not connected.");

  WiFiClientSecure client;
  client.setCACert(kGoogleTrustServicesRootR4);
  HTTPClient request;
  request.setConnectTimeout(kRequestTimeoutMs);
  request.setTimeout(kRequestTimeoutMs);
  request.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (!request.begin(client, url)) return failure(ProfileProbeState::UnexpectedResponse, 0, url, "Unable to start secure request.");
  request.addHeader("User-Agent", kUserAgent);
  request.addHeader("Accept", "text/html,application/xhtml+xml");
  const int status = request.GET();
  const String contentType = request.header("Content-Type");

  if (status == HTTP_CODE_OK) {
    if (!contentType.startsWith("text/html")) {
      request.end();
      return failure(ProfileProbeState::UnexpectedResponse, status, url, "Expected a public HTML profile page.");
    }
    ProfileProbe result = failure(ProfileProbeState::Reachable, status, url, "Public profile page reachable. Ambiguous aggregate counters remain hidden.");
    Stream* body = request.getStreamPtr();
    if (body != nullptr) parseProfileFields(*body, result);
    request.end();
    if (result.hasFollowerCount || result.hasFollowingCount || result.hasModelCount) {
      result.message = "Verified public profile totals were read from labelled page elements. Ambiguous aggregate counters remain hidden.";
    }
    return result;
  }
  request.end();
  if (status == HTTP_CODE_NOT_FOUND) return failure(ProfileProbeState::NotFound, status, url, "Profile was not found.");
  if (status == HTTP_CODE_UNAUTHORIZED || status == HTTP_CODE_FORBIDDEN) return failure(ProfileProbeState::AccessDenied, status, url, "MakerWorld denied direct access (possibly a browser challenge). No values were stored.");
  if (status == HTTP_CODE_TOO_MANY_REQUESTS) return failure(ProfileProbeState::RateLimited, status, url, "MakerWorld requested a slower refresh rate.");
  if (status >= 500 && status <= 599) return failure(ProfileProbeState::ServerError, status, url, "MakerWorld returned a server error.");
  if (status < 0) return failure(ProfileProbeState::Timeout, 0, url, "Network request timed out or failed.");
  return failure(ProfileProbeState::UnexpectedResponse, static_cast<uint16_t>(status), url, "Unexpected MakerWorld response.");
}

}  // namespace mwa
