#include "mwa/ProvisioningPortal.h"

#include <WiFi.h>

#include "mwa/ProfileInput.h"

namespace mwa {
namespace {
constexpr byte kDnsPort = 53;

String setupPassword() {
  const uint32_t value = esp_random() % 90000000UL + 10000000UL;
  return String(value);
}

String setupSsid() {
  const uint64_t mac = ESP.getEfuseMac();
  char suffix[5];
  snprintf(suffix, sizeof(suffix), "%04X", static_cast<uint16_t>(mac & 0xFFFF));
  return "MakerWorldAnalytics-" + String(suffix);
}
}  // namespace

ProvisioningPortal::ProvisioningPortal(ConfigStore& configStore) : configStore_(configStore) {}

bool ProvisioningPortal::begin(AppConfig& config, bool forceSetup) {
  if (active_) return true;
  config_ = &config;
  restartRequested_ = false;
  info_.ssid = setupSsid();
  info_.password = setupPassword();
  WiFi.mode(WIFI_AP_STA);
  if (!WiFi.softAP(info_.ssid.c_str(), info_.password.c_str())) return false;
  info_.address = WiFi.softAPIP();
  dns_.start(kDnsPort, "*", info_.address);
  server_.on("/", HTTP_GET, [this] { handleRoot(); });
  server_.on("/save", HTTP_POST, [this] { handleSave(); });
  server_.on("/health", HTTP_GET, [this] { server_.send(200, "application/json", "{\"status\":\"ok\"}"); });
  server_.onNotFound([this] { handleNotFound(); });
  server_.begin();
  active_ = true;
  Serial.printf("[MWA] portal: %s at http://%s%s\n", forceSetup ? "setup" : "settings", info_.address.toString().c_str(), "/");
  return true;
}

void ProvisioningPortal::tick() {
  if (!active_) return;
  dns_.processNextRequest();
  server_.handleClient();
}

void ProvisioningPortal::stop() {
  if (!active_) return;
  dns_.stop();
  server_.stop();
  WiFi.softAPdisconnect(true);
  active_ = false;
}

bool ProvisioningPortal::active() const { return active_; }
bool ProvisioningPortal::restartRequested() const { return restartRequested_; }
const ProvisioningInfo& ProvisioningPortal::info() const { return info_; }

String ProvisioningPortal::escaped(const String& input) const {
  String output;
  output.reserve(input.length());
  for (const char c : input) {
    if (c == '&') output += "&amp;";
    else if (c == '<') output += "&lt;";
    else if (c == '>') output += "&gt;";
    else if (c == '"') output += "&quot;";
    else output += c;
  }
  return output;
}

String ProvisioningPortal::htmlPage(const String& message) const {
  const String profile = config_ == nullptr ? "" : escaped(config_->profileUsername);
  const String timezone = config_ == nullptr ? "UTC" : escaped(config_->timezone);
  const String refresh = config_ == nullptr ? String(kDefaultRefreshMinutes) : String(config_->refreshMinutes);
  String page;
  page.reserve(4600);
  page += F("<!doctype html><html lang='en'><head><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'>");
  page += F("<title>MakerWorldAnalytics setup</title><style>body{margin:0;background:#0b0d11;color:#f4f7fb;font:16px system-ui}.shell{max-width:560px;margin:0 auto;padding:24px}.card{background:#171b22;border:1px solid #303846;border-radius:16px;padding:20px;margin:16px 0}h1{margin:0;color:#f4f7fb}em{color:#4ade80;font-style:normal}label{display:block;margin:14px 0 6px}input{box-sizing:border-box;width:100%;background:#0b0d11;color:#f4f7fb;border:1px solid #465164;border-radius:9px;padding:12px;font:inherit}button{margin-top:20px;border:0;border-radius:9px;background:#2563eb;color:white;padding:12px 16px;font-weight:700;font:inherit}.note{color:#b7c0cf;font-size:.9rem}.ok{color:#4ade80}.warn{color:#fbbf24}</style></head><body><main class='shell'><h1>MakerWorld<em>Analytics</em></h1><p class='note'>Private by design. Configuration stays on this device; MakerWorld data is fetched directly from it.</p>");
  if (!message.isEmpty()) page += "<p class='ok'>" + escaped(message) + "</p>";
  page += F("<section class='card'><strong>1. Connect Wi-Fi</strong><form method='post' action='/save'><label for='ssid'>Network name (SSID)</label><input id='ssid' name='ssid' required maxlength='32' autocomplete='username'><label for='password'>Wi-Fi password</label><input id='password' name='password' type='password' required maxlength='63' autocomplete='current-password'><p class='note'>The password is never shown or logged after submission.</p><strong>2. Public MakerWorld profile</strong><label for='profile'>Username or public profile URL</label><input id='profile' name='profile' value='");
  page += profile;
  page += F("' maxlength='128' placeholder='username or makerworld.com/en/@username'><label for='timezone'>Timezone</label><input id='timezone' name='timezone' value='");
  page += timezone;
  page += F("' maxlength='48'><label for='refresh'>Refresh interval (minutes)</label><input id='refresh' name='refresh' type='number' min='15' max='1440' value='");
  page += refresh;
  page += F("'><button type='submit'>Save and restart</button></form></section><section class='card note'><span class='warn'>Recovery:</span> hold BOOT during startup to reopen setup. This is an independent, unofficial project and is not affiliated with MakerWorld or Bambu Lab.</section></main></body></html>");
  return page;
}

void ProvisioningPortal::handleRoot() { server_.send(200, "text/html; charset=utf-8", htmlPage("")); }

void ProvisioningPortal::handleSave() {
  if (config_ == nullptr || !server_.hasArg("ssid") || !server_.hasArg("password") || !server_.hasArg("profile")) {
    server_.send(400, "text/html; charset=utf-8", htmlPage("Complete the required fields."));
    return;
  }
  const ProfileInputResult profile = normalizeProfileInput(server_.arg("profile").c_str());
  if (!profile.valid) {
    server_.send(400, "text/html; charset=utf-8", htmlPage(profile.error.c_str()));
    return;
  }
  uint16_t refresh = server_.arg("refresh").toInt();
  if (refresh < kMinimumRefreshMinutes || refresh > kMaximumRefreshMinutes) {
    server_.send(400, "text/html; charset=utf-8", htmlPage("Choose a refresh interval from 15 to 1440 minutes."));
    return;
  }
  config_->profileUsername = profile.username.c_str();
  config_->timezone = server_.arg("timezone");
  config_->refreshMinutes = refresh;
  config_->portalEnabled = false;
  const bool saved = configStore_.saveWiFiCredentials(server_.arg("ssid"), server_.arg("password")) && configStore_.save(*config_);
  if (!saved) {
    server_.send(500, "text/html; charset=utf-8", htmlPage("Settings could not be saved. Try again."));
    return;
  }
  restartRequested_ = true;
  server_.send(200, "text/html; charset=utf-8", htmlPage("Saved. The device will restart and connect to Wi-Fi."));
}

void ProvisioningPortal::handleNotFound() {
  server_.sendHeader("Location", String("http://") + info_.address.toString() + "/", true);
  server_.send(302, "text/plain", "");
}

}  // namespace mwa
