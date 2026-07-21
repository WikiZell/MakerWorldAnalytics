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

 private:
  void setAccent(lv_color_t color);
  lv_obj_t* title_{nullptr};
  lv_obj_t* state_{nullptr};
  lv_obj_t* profile_{nullptr};
  lv_obj_t* summary_{nullptr};
  lv_obj_t* detail_{nullptr};
  lv_obj_t* footer_{nullptr};
};

}  // namespace mwa
