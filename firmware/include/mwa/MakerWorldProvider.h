#pragma once

#include <Arduino.h>

namespace mwa {

enum class ProfileProbeState : uint8_t {
  Idle,
  Reachable,
  InvalidInput,
  NetworkUnavailable,
  Timeout,
  NotFound,
  AccessDenied,
  RateLimited,
  ServerError,
  UnexpectedResponse,
};

struct ProfileProbe {
  ProfileProbeState state{ProfileProbeState::Idle};
  uint16_t httpStatus{0};
  String profileUrl{};
  String message{};
  bool hasFollowerCount{false};
  bool hasFollowingCount{false};
  bool hasModelCount{false};
  uint32_t followers{0};
  uint32_t following{0};
  uint32_t models{0};
};

class MakerWorldProvider {
 public:
  ProfileProbe probePublicProfile(const String& usernameOrUrl) const;
  static String publicProfileUrl(const String& username);
};

}  // namespace mwa
