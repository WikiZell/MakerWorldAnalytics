#include "mwa/ConfigStore.h"

#include <Preferences.h>

namespace mwa {
namespace {
constexpr char kSettingsNamespace[] = "mwa-settings";
constexpr char kWifiNamespace[] = "mwa-wifi";

uint16_t clampedRefresh(uint16_t value) {
  if (value < kMinimumRefreshMinutes) return kMinimumRefreshMinutes;
  if (value > kMaximumRefreshMinutes) return kMaximumRefreshMinutes;
  return value;
}
}  // namespace

bool ConfigStore::begin() {
  // Create both namespaces once in read/write mode. Opening a nonexistent
  // namespace read-only causes noisy first-boot errors on ESP-IDF NVS.
  Preferences settings;
  if (!settings.begin(kSettingsNamespace, false)) return false;
  settings.end();
  Preferences wifi;
  if (!wifi.begin(kWifiNamespace, false)) return false;
  wifi.end();
  initialized_ = true;
  return true;
}

bool ConfigStore::load(AppConfig& config) {
  if (!initialized_) return false;
  Preferences preferences;
  if (!preferences.begin(kSettingsNamespace, true)) return false;
  const uint16_t version = preferences.getUShort("schema", 0);
  if (version == 0) {
    preferences.end();
    config = AppConfig{};
    return true;
  }
  if (version > kConfigSchemaVersion) {
    preferences.end();
    return false;
  }
  config.schemaVersion = kConfigSchemaVersion;
  config.deviceName = preferences.getString("name", "MakerWorldAnalytics");
  config.profileUsername = preferences.getString("profile", "");
  config.timezone = preferences.getString("tz", "UTC");
  config.refreshMinutes = clampedRefresh(preferences.getUShort("refresh", kDefaultRefreshMinutes));
  config.brightness = preferences.getUChar("bright", 180);
  config.theme = static_cast<Theme>(preferences.getUChar("theme", static_cast<uint8_t>(Theme::Dark)));
  config.updateChannel = static_cast<UpdateChannel>(preferences.getUChar("channel", static_cast<uint8_t>(UpdateChannel::Stable)));
  config.use24HourClock = preferences.getBool("clock24", true);
  config.automaticUpdateChecks = preferences.getBool("auto-ota", true);
  config.portalEnabled = preferences.getBool("portal", false);
  preferences.end();
  return true;
}

bool ConfigStore::save(const AppConfig& config) {
  if (!initialized_) return false;
  Preferences preferences;
  if (!preferences.begin(kSettingsNamespace, false)) return false;
  bool success = true;
  success &= preferences.putUShort("schema", kConfigSchemaVersion) > 0;
  success &= preferences.putString("name", config.deviceName) > 0;
  success &= preferences.putString("profile", config.profileUsername) >= 0;
  success &= preferences.putString("tz", config.timezone) > 0;
  success &= preferences.putUShort("refresh", clampedRefresh(config.refreshMinutes)) > 0;
  success &= preferences.putUChar("bright", config.brightness) > 0;
  success &= preferences.putUChar("theme", static_cast<uint8_t>(config.theme)) > 0;
  success &= preferences.putUChar("channel", static_cast<uint8_t>(config.updateChannel)) > 0;
  success &= preferences.putBool("clock24", config.use24HourClock) > 0;
  success &= preferences.putBool("auto-ota", config.automaticUpdateChecks) > 0;
  success &= preferences.putBool("portal", config.portalEnabled) > 0;
  preferences.end();
  return success;
}

bool ConfigStore::clearAll() {
  Preferences preferences;
  if (!preferences.begin(kSettingsNamespace, false)) return false;
  const bool settingsCleared = preferences.clear();
  preferences.end();
  return settingsCleared && clearWiFiCredentials();
}

bool ConfigStore::hasWiFiCredentials() const {
  Preferences preferences;
  if (!preferences.begin(kWifiNamespace, true)) return false;
  const bool hasCredentials = preferences.isKey("ssid") && !preferences.getString("ssid", "").isEmpty();
  preferences.end();
  return hasCredentials;
}

bool ConfigStore::saveWiFiCredentials(const String& ssid, const String& password) {
  if (ssid.isEmpty() || password.isEmpty()) return false;
  Preferences preferences;
  if (!preferences.begin(kWifiNamespace, false)) return false;
  const bool success = preferences.putString("ssid", ssid) > 0 && preferences.putString("pass", password) >= 0;
  preferences.end();
  return success;
}

bool ConfigStore::loadWiFiCredentials(String& ssid, String& password) const {
  Preferences preferences;
  if (!preferences.begin(kWifiNamespace, true)) return false;
  if (!preferences.isKey("ssid")) {
    preferences.end();
    return false;
  }
  ssid = preferences.getString("ssid", "");
  password = preferences.isKey("pass") ? preferences.getString("pass", "") : "";
  preferences.end();
  return !ssid.isEmpty();
}

bool ConfigStore::clearWiFiCredentials() {
  Preferences preferences;
  if (!preferences.begin(kWifiNamespace, false)) return false;
  const bool success = preferences.clear();
  preferences.end();
  return success;
}

}  // namespace mwa
