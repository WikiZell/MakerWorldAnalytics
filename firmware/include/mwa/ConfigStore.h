#pragma once

#include "mwa/AppConfig.h"

namespace mwa {

class ConfigStore {
 public:
  bool begin();
  bool load(AppConfig& config);
  bool save(const AppConfig& config);
  bool clearAll();
  bool hasWiFiCredentials() const;
  bool saveWiFiCredentials(const String& ssid, const String& password);
  bool loadWiFiCredentials(String& ssid, String& password) const;
  bool clearWiFiCredentials();

 private:
  bool initialized_{false};
};

}  // namespace mwa
