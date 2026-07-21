#include "mwa/Dashboard.h"

namespace mwa {
namespace {
constexpr uint32_t kBackground = 0x0B0D11;
constexpr uint32_t kCard = 0x171B22;
constexpr uint32_t kBorder = 0x303846;
constexpr uint32_t kText = 0xF4F7FB;
constexpr uint32_t kMuted = 0xB7C0CF;
constexpr uint32_t kGreen = 0x4ADE80;
constexpr uint32_t kAmber = 0xFBBF24;
constexpr uint32_t kBlue = 0x60A5FA;

lv_obj_t* createCard(lv_obj_t* parent, int16_t x, int16_t y, int16_t width, int16_t height) {
  lv_obj_t* card = lv_obj_create(parent);
  lv_obj_set_pos(card, x, y);
  lv_obj_set_size(card, width, height);
  lv_obj_set_style_radius(card, 12, 0);
  lv_obj_set_style_bg_color(card, lv_color_hex(kCard), 0);
  lv_obj_set_style_border_color(card, lv_color_hex(kBorder), 0);
  lv_obj_set_style_pad_all(card, 10, 0);
  return card;
}
}  // namespace

void Dashboard::begin() {
  lv_obj_t* root = lv_screen_active();
  lv_obj_clean(root);
  lv_obj_set_style_bg_color(root, lv_color_hex(kBackground), 0);
  lv_obj_set_style_text_color(root, lv_color_hex(kText), 0);

  title_ = lv_label_create(root);
  lv_label_set_text(title_, "MakerWorldAnalytics");
  lv_obj_set_style_text_color(title_, lv_color_hex(kText), 0);
  lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, 8);
  state_ = lv_label_create(root);
  lv_obj_set_style_text_color(state_, lv_color_hex(kAmber), 0);
  lv_obj_align(state_, LV_ALIGN_TOP_RIGHT, -10, 8);

  lv_obj_t* profileCard = createCard(root, 8, 36, 304, 76);
  profile_ = lv_label_create(profileCard);
  lv_obj_set_style_text_color(profile_, lv_color_hex(kText), 0);
  summary_ = lv_label_create(profileCard);
  lv_obj_set_style_text_color(summary_, lv_color_hex(kMuted), 0);
  lv_obj_align(summary_, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_t* detailCard = createCard(root, 8, 120, 304, 82);
  detail_ = lv_label_create(detailCard);
  lv_obj_set_width(detail_, 282);
  lv_label_set_long_mode(detail_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(detail_, lv_color_hex(kMuted), 0);

  footer_ = lv_label_create(root);
  lv_label_set_text(footer_, "HOME   MODELS   TRENDS   SETTINGS");
  lv_obj_set_style_text_color(footer_, lv_color_hex(kBlue), 0);
  lv_obj_align(footer_, LV_ALIGN_BOTTOM_MID, 0, -8);
  showSystemStatus("Starting services…");
}

void Dashboard::setAccent(lv_color_t color) { lv_obj_set_style_text_color(state_, color, 0); }

void Dashboard::showProvisioning(const ProvisioningInfo& info) {
  setAccent(lv_color_hex(kAmber));
  lv_label_set_text(state_, "SETUP");
  lv_label_set_text(profile_, "Connect to device setup Wi-Fi");
  lv_label_set_text_fmt(summary_, "%s", info.ssid.c_str());
  lv_label_set_text_fmt(detail_, "Password: %s\nOpen: http://%s\nEnter your Wi-Fi and public MakerWorld profile.", info.password.c_str(), info.address.toString().c_str());
  lv_label_set_text(footer_, "SETUP   •   PRIVACY FIRST");
}

void Dashboard::showConnecting(const AppConfig& config, const String& ssid) {
  setAccent(lv_color_hex(kAmber));
  lv_label_set_text(state_, "CONNECTING");
  const String profile = config.profileUsername.isEmpty() ? "Profile needs setup" : String("@") + config.profileUsername;
  lv_label_set_text(profile_, profile.c_str());
  lv_label_set_text_fmt(summary_, "Wi-Fi: %s • refresh every %u min", ssid.c_str(), config.refreshMinutes);
  lv_label_set_text(detail_, "Connecting securely. Public profile data is requested directly from this device; no credentials or analytics are sent to WikiZell.");
  lv_label_set_text(footer_, "HOME   MODELS   TRENDS   SETTINGS");
}

void Dashboard::showReady(const AppConfig& config, const ProfileProbe& probe) {
  const bool reachable = probe.state == ProfileProbeState::Reachable;
  setAccent(lv_color_hex(reachable ? kGreen : kAmber));
  lv_label_set_text(state_, reachable ? "ONLINE" : "CHECK PROFILE");
  const String profile = config.profileUsername.isEmpty() ? "No public profile configured" : String("@") + config.profileUsername;
  lv_label_set_text(profile_, profile.c_str());
  lv_label_set_text_fmt(summary_, "Wi-Fi connected • refresh every %u min", config.refreshMinutes);
  lv_label_set_text_fmt(detail_, "%s\nOnly verified public fields will be shown; unverified aggregate counters remain hidden.", probe.message.c_str());
  lv_label_set_text(footer_, "HOME   MODELS   TRENDS   SETTINGS");
}

void Dashboard::showSystemStatus(const String& message) {
  setAccent(lv_color_hex(kAmber));
  lv_label_set_text(state_, "STARTING");
  lv_label_set_text(profile_, "MakerWorldAnalytics");
  lv_label_set_text(summary_, "Created by Rene Girardi • WikiZell");
  lv_label_set_text(detail_, message.c_str());
}

}  // namespace mwa
