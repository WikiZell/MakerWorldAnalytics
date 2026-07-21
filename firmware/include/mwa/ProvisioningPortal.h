#pragma once

#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>

#include "mwa/AppConfig.h"
#include "mwa/ConfigStore.h"

namespace mwa {

struct ProvisioningInfo {
  String ssid{};
  String password{};
  IPAddress address{};
};

class ProvisioningPortal {
 public:
  explicit ProvisioningPortal(ConfigStore& configStore);
  bool begin(AppConfig& config, bool forceSetup);
  void tick();
  void stop();
  bool active() const;
  bool restartRequested() const;
  const ProvisioningInfo& info() const;

 private:
  String htmlPage(const String& message) const;
  void handleRoot();
  void handleSave();
  void handleScan();
  void handleNotFound();
  String escaped(const String& input) const;

  ConfigStore& configStore_;
  WebServer server_{80};
  DNSServer dns_;
  AppConfig* config_{nullptr};
  ProvisioningInfo info_;
  bool active_{false};
  bool setupMode_{true};
  bool restartRequested_{false};
};

}  // namespace mwa
