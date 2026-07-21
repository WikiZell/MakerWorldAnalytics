#include <Arduino.h>
#include <WiFi.h>
#include <esp_heap_caps.h>
#include <lvgl.h>

#include "mwa/AppConfig.h"
#include "mwa/Board.h"
#include "mwa/ConfigStore.h"
#include "mwa/Dashboard.h"
#include "mwa/ProfileRefreshService.h"
#include "mwa/ProvisioningPortal.h"

namespace {
constexpr uint16_t kWidth = 320;
constexpr uint16_t kHeight = 240;
constexpr uint16_t kDrawRows = 24;
constexpr uint8_t kBootButtonPin = 0;
constexpr uint32_t kWiFiConnectionTimeoutMs = 30000;
constexpr uint32_t kRecoveryHoldMs = 5000;
constexpr uint32_t kFactoryResetHoldMs = 10000;
constexpr uint8_t kMaxWifiNetworks = 4;
lv_color_t drawBuffer[kWidth * kDrawRows];
lv_display_t* lvDisplay = nullptr;
mwa::ConfigStore configStore;
mwa::AppConfig config;
mwa::ProvisioningPortal portal(configStore);
mwa::Dashboard dashboard;
mwa::ProfileRefreshService profileRefresh;
String configuredSsid;
bool profileRequested = false;
uint32_t restartAt = 0;
uint32_t wifiConnectStartedAt = 0;
uint32_t nextProfileRefreshAt = 0;
bool wifiScanInProgress = false;
bool directJoinInProgress = false;
String directJoinSsid;
String directJoinPassword;
String scannedSsids[kMaxWifiNetworks];

enum class BootRecovery { None, Setup, FactoryReset };

void flush(lv_display_t*, const lv_area_t* area, uint8_t* pixels) {
  const uint16_t width = area->x2 - area->x1 + 1;
  const uint16_t height = area->y2 - area->y1 + 1;
  mwa::display.startWrite();
  mwa::display.setAddrWindow(area->x1, area->y1, width, height);
  mwa::display.writePixels(reinterpret_cast<lgfx::rgb565_t*>(pixels), width * height);
  mwa::display.endWrite();
  lv_display_flush_ready(lvDisplay);
}

void readTouch(lv_indev_t*, lv_indev_data_t* data) {
  if (!mwa::touch.touched()) {
    data->state = LV_INDEV_STATE_RELEASED;
    return;
  }
  TS_Point point = mwa::touch.getPoint();
  data->point.x = point.x;
  data->point.y = point.y;
  data->state = LV_INDEV_STATE_PRESSED;
}

BootRecovery checkBootRecovery() {
  if (digitalRead(kBootButtonPin) != LOW) return BootRecovery::None;
  const uint32_t startedAt = millis();
  dashboard.showSystemStatus("BOOT held. Keep holding 5 s for setup or 10 s to erase local settings.");
  while (digitalRead(kBootButtonPin) == LOW && millis() - startedAt < kFactoryResetHoldMs) {
    const uint32_t elapsed = millis() - startedAt;
    if (elapsed >= kRecoveryHoldMs) dashboard.showSystemStatus("Recovery armed. Keep holding until 10 s to factory reset.");
    lv_timer_handler();
    delay(20);
  }
  const uint32_t elapsed = millis() - startedAt;
  if (elapsed >= kFactoryResetHoldMs) return BootRecovery::FactoryReset;
  return elapsed >= kRecoveryHoldMs ? BootRecovery::Setup : BootRecovery::None;
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println("[MWA] boot: esp32-2432s028r / 0.1.0-alpha.1");
  pinMode(kBootButtonPin, INPUT_PULLUP);
  configStore.begin();
  const bool configLoaded = configStore.load(config);
  if (!configLoaded) config = mwa::AppConfig{};
  mwa::display.init();
  mwa::display.setRotation(1);
  mwa::display.setBrightness(config.brightness);
  mwa::touch.begin();
  mwa::touch.setRotation(1);
  lv_init();
  lvDisplay = lv_display_create(kWidth, kHeight);
  lv_display_set_flush_cb(lvDisplay, flush);
  lv_display_set_buffers(lvDisplay, drawBuffer, nullptr, sizeof(drawBuffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_indev_t* input = lv_indev_create();
  lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(input, readTouch);
  dashboard.begin(config.theme);
  if (!configLoaded) dashboard.showSystemStatus("Configuration needs recovery. Starting secure local setup.");

  String password;
  const bool hasCredentials = configStore.loadWiFiCredentials(configuredSsid, password);
  const BootRecovery recovery = checkBootRecovery();
  if (recovery == BootRecovery::FactoryReset) {
    configStore.clearAll();
    config = mwa::AppConfig{};
    configuredSsid = "";
    password = "";
    dashboard.showSystemStatus("Local settings erased. Starting secure setup.");
  }
  const bool needsSetup = recovery != BootRecovery::None;
  if (needsSetup) {
    if (portal.begin(config, true)) dashboard.showProvisioning(portal.info());
    else dashboard.showSystemStatus("Setup Wi-Fi could not start. Hold BOOT and restart the device.");
  } else if (!hasCredentials) {
    dashboard.showWifiMenu("");
  } else {
    WiFi.mode(WIFI_STA);
    WiFi.begin(configuredSsid.c_str(), password.c_str());
    wifiConnectStartedAt = millis();
    dashboard.showConnecting(config, configuredSsid);
  }
  Serial.printf("[MWA] memory: free=%u min=%u largest=%u\n", ESP.getFreeHeap(), ESP.getMinFreeHeap(), heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
  Serial.println("[MWA] ui: dashboard ready; no MakerWorld profile compiled in");
}

void loop() {
  lv_timer_handler();
  portal.tick();
  if (!portal.active() && dashboard.takeSettingsRequest()) {
    if (portal.begin(config, false)) dashboard.showProvisioning(portal.info());
    else dashboard.showSystemStatus("Setup Wi-Fi could not start. Restart while holding BOOT.");
  }
  if (portal.restartRequested() && restartAt == 0) restartAt = millis() + 1500;
  if (restartAt != 0 && millis() >= restartAt) ESP.restart();
  if (!portal.active() && dashboard.takeWiFiScanRequest() && !wifiScanInProgress) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(false, false);
    WiFi.scanDelete();
    if (WiFi.scanNetworks(true) == WIFI_SCAN_RUNNING) {
      wifiScanInProgress = true;
      dashboard.showWifiScanning();
      Serial.println("[MWA] wifi: scanning");
    } else {
      dashboard.showWifiScanResults(scannedSsids, 0);
    }
  }
  if (wifiScanInProgress) {
    const int16_t found = WiFi.scanComplete();
    if (found >= 0) {
      uint8_t count = 0;
      for (int16_t index = 0; index < found && count < kMaxWifiNetworks; ++index) {
        const String candidate = WiFi.SSID(index);
        bool duplicate = candidate.isEmpty();
        for (uint8_t seen = 0; seen < count; ++seen) duplicate = duplicate || scannedSsids[seen] == candidate;
        if (!duplicate) scannedSsids[count++] = candidate;
      }
      WiFi.scanDelete();
      wifiScanInProgress = false;
      dashboard.showWifiScanResults(scannedSsids, count);
      Serial.printf("[MWA] wifi: found %u network(s)\n", count);
    }
  }
  if (!portal.active() && dashboard.takeForgetWiFiRequest()) {
    WiFi.disconnect(true, true);
    configStore.clearWiFiCredentials();
    configuredSsid = "";
    dashboard.showWifiMenu("");
    Serial.println("[MWA] wifi: saved network forgotten");
  }
  if (!portal.active() && dashboard.takeResetRequest()) {
    WiFi.disconnect(true, true);
    configStore.clearWiFiCredentials();
    configuredSsid = "";
    dashboard.showWifiMenu("");
    Serial.println("[MWA] wifi: network settings reset");
  }
  if (!portal.active()) {
    String ssid;
    String password;
    if (dashboard.takeJoinRequest(ssid, password)) {
      directJoinSsid = ssid;
      directJoinPassword = password;
      directJoinInProgress = true;
      WiFi.mode(WIFI_STA);
      WiFi.begin(directJoinSsid.c_str(), directJoinPassword.c_str());
      wifiConnectStartedAt = millis();
      dashboard.showConnecting(config, directJoinSsid);
      Serial.printf("[MWA] wifi: joining %s\n", directJoinSsid.c_str());
    }
  }
  if (!portal.active() && WiFi.status() != WL_CONNECTED && wifiConnectStartedAt != 0 && millis() - wifiConnectStartedAt >= kWiFiConnectionTimeoutMs) {
    wifiConnectStartedAt = 0;
    if (directJoinInProgress) {
      directJoinInProgress = false;
      directJoinPassword = "";
      dashboard.showWifiMenu("");
      Serial.println("[MWA] wifi: direct join timed out");
    } else {
      dashboard.showWifiMenu(configuredSsid);
      Serial.println("[MWA] wifi: saved connection timed out");
    }
  }
  if (!portal.active() && WiFi.status() == WL_CONNECTED && directJoinInProgress) {
    if (configStore.saveWiFiCredentials(directJoinSsid, directJoinPassword)) {
      configuredSsid = directJoinSsid;
      dashboard.showSystemStatus("Wi-Fi joined and saved. Open SETUP any time to change it.");
      Serial.printf("[MWA] wifi: joined %s\n", configuredSsid.c_str());
    } else {
      dashboard.showSystemStatus("Wi-Fi joined, but it could not be saved. Try again from SETUP.");
      Serial.println("[MWA] wifi: joined but credential save failed");
    }
    directJoinPassword = "";
    directJoinInProgress = false;
    wifiConnectStartedAt = 0;
  }
  if (!portal.active() && WiFi.status() == WL_CONNECTED && !config.profileUsername.isEmpty() && !profileRequested && millis() >= nextProfileRefreshAt) {
    profileRequested = profileRefresh.request(config.profileUsername);
    if (profileRequested) dashboard.showConnecting(config, configuredSsid);
  }
  mwa::ProfileProbe probe;
  if (profileRefresh.take(probe)) {
    profileRequested = false;
    nextProfileRefreshAt = millis() + static_cast<uint32_t>(config.refreshMinutes) * 60UL * 1000UL;
    dashboard.showReady(config, probe);
    Serial.printf("[MWA] profile probe: state=%u http=%u\n", static_cast<unsigned>(probe.state), probe.httpStatus);
  }
  delay(5);
}
