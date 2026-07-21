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
};

class MakerWorldProvider {
 public:
  ProfileProbe probePublicProfile(const String& usernameOrUrl) const;
  static String publicProfileUrl(const String& username);
};

}  // namespace mwa
