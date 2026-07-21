#pragma once

#include <lvgl.h>

#include "mwa/AppConfig.h"
#include "mwa/MakerWorldProvider.h"
#include "mwa/ProvisioningPortal.h"

namespace mwa {

class Dashboard {
 public:
  void begin(Theme theme);
  void showProvisioning(const ProvisioningInfo& info);
  void showConnecting(const AppConfig& config, const String& ssid);
  void showReady(const AppConfig& config, const ProfileProbe& probe);
  void showSystemStatus(const String& message);
  void showWifiMenu(const String& currentSsid);
  void showWifiScanResults(const String* ssids, uint8_t count);
  void showWifiScanning();
  bool takeSettingsRequest();
  bool takeWiFiScanRequest();
  bool takeForgetWiFiRequest();
  bool takeResetRequest();
  bool takeJoinRequest(String& ssid, String& password);

 private:
  enum class Screen : uint8_t { Home, Models, Trends, Settings };
  enum class Action : uint8_t { Portal, Scan, Forget, Reset, Join };
  struct ActionRequest {
    Dashboard* dashboard{nullptr};
    uint8_t action{0};
    String value{};
  };
  static void onNav(lv_event_t* event);
  static void onAction(lv_event_t* event);
  static void onKeyboard(lv_event_t* event);
  void showScreen(Screen screen);
  void clearActionPanel();
  void addActionButton(const char* label, Action action, int16_t x, int16_t y, int16_t width, const String& value = "");
  void showJoinPassword(const String& ssid);
  void closeKeyboard();
  void setAccent(lv_color_t color);
  lv_obj_t* title_{nullptr};
  lv_obj_t* state_{nullptr};
  lv_obj_t* profile_{nullptr};
  lv_obj_t* summary_{nullptr};
  lv_obj_t* detail_{nullptr};
  lv_obj_t* footer_{nullptr};
  lv_obj_t* actionPanel_{nullptr};
  lv_obj_t* passwordInput_{nullptr};
  lv_obj_t* keyboard_{nullptr};
  Screen screen_{Screen::Home};
  bool settingsRequested_{false};
  bool wifiScanRequested_{false};
  bool forgetWiFiRequested_{false};
  bool resetRequested_{false};
  bool joinRequested_{false};
  String pendingSsid_{};
  String pendingPassword_{};
  String currentSsid_{};
  bool confirmForget_{false};
  bool confirmReset_{false};
  ActionRequest actionRequests_[6]{};
  uint8_t actionCount_{0};
  AppConfig lastConfig_{};
  ProfileProbe lastProbe_{};
  bool hasProbe_{false};
};

}  // namespace mwa
