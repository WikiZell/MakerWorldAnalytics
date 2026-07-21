#pragma once

#include <Arduino.h>

namespace mwa {

constexpr uint16_t kConfigSchemaVersion = 1;
constexpr uint16_t kDefaultRefreshMinutes = 15;
constexpr uint16_t kMinimumRefreshMinutes = 15;
constexpr uint16_t kMaximumRefreshMinutes = 24 * 60;

enum class Theme : uint8_t { Dark = 0, Light = 1 };
enum class UpdateChannel : uint8_t { Stable = 0, Beta = 1 };

struct AppConfig {
  uint16_t schemaVersion{kConfigSchemaVersion};
  String deviceName{"MakerWorldAnalytics"};
  String profileUsername{};
  String timezone{"UTC"};
  uint16_t refreshMinutes{kDefaultRefreshMinutes};
  uint8_t brightness{180};
  Theme theme{Theme::Dark};
  UpdateChannel updateChannel{UpdateChannel::Stable};
  bool use24HourClock{true};
  bool automaticUpdateChecks{true};
  bool portalEnabled{false};
};

}  // namespace mwa
