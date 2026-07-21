#pragma once

#include <string>

namespace mwa {

struct ProfileInputResult {
  bool valid{false};
  std::string username;
  std::string error;
};

ProfileInputResult normalizeProfileInput(const std::string& raw);

}  // namespace mwa
