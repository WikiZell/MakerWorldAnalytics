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
  request.end();

  if (status == HTTP_CODE_OK) {
    if (!contentType.startsWith("text/html")) return failure(ProfileProbeState::UnexpectedResponse, status, url, "Expected a public HTML profile page.");
    return failure(ProfileProbeState::Reachable, status, url, "Public profile page reachable. Metrics await a verified provider mapping.");
  }
  if (status == HTTP_CODE_NOT_FOUND) return failure(ProfileProbeState::NotFound, status, url, "Profile was not found.");
  if (status == HTTP_CODE_UNAUTHORIZED || status == HTTP_CODE_FORBIDDEN) return failure(ProfileProbeState::AccessDenied, status, url, "Public profile access was denied.");
  if (status == HTTP_CODE_TOO_MANY_REQUESTS) return failure(ProfileProbeState::RateLimited, status, url, "MakerWorld requested a slower refresh rate.");
  if (status >= 500 && status <= 599) return failure(ProfileProbeState::ServerError, status, url, "MakerWorld returned a server error.");
  if (status < 0) return failure(ProfileProbeState::Timeout, 0, url, "Network request timed out or failed.");
  return failure(ProfileProbeState::UnexpectedResponse, static_cast<uint16_t>(status), url, "Unexpected MakerWorld response.");
}

}  // namespace mwa
