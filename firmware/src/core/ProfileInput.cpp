#include "mwa/ProfileInput.h"

#include <algorithm>
#include <cctype>

namespace mwa {
namespace {

std::string trim(std::string value) {
  const auto first = std::find_if_not(value.begin(), value.end(), [](unsigned char c) { return std::isspace(c); });
  const auto last = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char c) { return std::isspace(c); }).base();
  return first >= last ? "" : std::string(first, last);
}

bool validUsername(const std::string& value) {
  if (value.empty() || value.size() > 64) return false;
  return std::all_of(value.begin(), value.end(), [](unsigned char c) {
    return std::isalnum(c) || c == '_' || c == '-';
  });
}

}  // namespace

ProfileInputResult normalizeProfileInput(const std::string& raw) {
  std::string value = trim(raw);
  const std::string domain = "makerworld.com/";
  const auto domainPos = value.find(domain);
  if (domainPos != std::string::npos) {
    value = value.substr(domainPos + domain.size());
    const auto at = value.find('@');
    if (at != std::string::npos) value = value.substr(at + 1);
  }
  if (!value.empty() && value.front() == '@') value.erase(0, 1);
  const auto end = value.find_first_of("/?#");
  if (end != std::string::npos) value.resize(end);
  ProfileInputResult result;
  if (!validUsername(value)) {
    result.error = "Enter a MakerWorld username or public profile URL.";
    return result;
  }
  result.valid = true;
  result.username = value;
  return result;
}

}  // namespace mwa
