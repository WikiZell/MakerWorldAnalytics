#pragma once

#include <lvgl.h>

#include "mwa/AppConfig.h"
#include "mwa/MakerWorldProvider.h"
#include "mwa/ProvisioningPortal.h"

namespace mwa {

class Dashboard {
 public:
  void begin();
  void showProvisioning(const ProvisioningInfo& info);
  void showConnecting(const AppConfig& config, const String& ssid);
  void showReady(const AppConfig& config, const ProfileProbe& probe);
  void showSystemStatus(const String& message);
  bool takeSettingsRequest();

 private:
  enum class Screen : uint8_t { Home, Models, Trends, Settings };
  static void onNav(lv_event_t* event);
  void showScreen(Screen screen);
  void setAccent(lv_color_t color);
  lv_obj_t* title_{nullptr};
  lv_obj_t* state_{nullptr};
  lv_obj_t* profile_{nullptr};
  lv_obj_t* summary_{nullptr};
  lv_obj_t* detail_{nullptr};
  lv_obj_t* footer_{nullptr};
  Screen screen_{Screen::Home};
  bool settingsRequested_{false};
  AppConfig lastConfig_{};
  ProfileProbe lastProbe_{};
  bool hasProbe_{false};
};

}  // namespace mwa
