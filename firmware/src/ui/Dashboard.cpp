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
bool lightTheme = false;

uint32_t themed(uint32_t dark, uint32_t light) { return lightTheme ? light : dark; }

struct NavAction { Dashboard* dashboard; uint8_t screen; };

lv_obj_t* createCard(lv_obj_t* parent, int16_t x, int16_t y, int16_t width, int16_t height) {
  lv_obj_t* card = lv_obj_create(parent);
  lv_obj_set_pos(card, x, y);
  lv_obj_set_size(card, width, height);
  lv_obj_set_style_radius(card, 12, 0);
  lv_obj_set_style_bg_color(card, lv_color_hex(themed(kCard, 0xFFFFFF)), 0);
  lv_obj_set_style_border_color(card, lv_color_hex(themed(kBorder, 0xCBD5E1)), 0);
  lv_obj_set_style_pad_all(card, 10, 0);
  return card;
}
}  // namespace

void Dashboard::begin(Theme theme) {
  lightTheme = theme == Theme::Light;
  lv_obj_t* root = lv_screen_active();
  lv_obj_clean(root);
  lv_obj_set_style_bg_color(root, lv_color_hex(themed(kBackground, 0xF1F5F9)), 0);
  lv_obj_set_style_text_color(root, lv_color_hex(themed(kText, 0x111827)), 0);

  title_ = lv_label_create(root);
  lv_label_set_text(title_, "MakerWorldAnalytics");
  lv_obj_set_style_text_color(title_, lv_color_hex(themed(kText, 0x111827)), 0);
  lv_obj_align(title_, LV_ALIGN_TOP_MID, 0, 8);
  state_ = lv_label_create(root);
  lv_obj_set_style_text_color(state_, lv_color_hex(kAmber), 0);
  lv_obj_align(state_, LV_ALIGN_TOP_RIGHT, -10, 8);

  lv_obj_t* profileCard = createCard(root, 8, 36, 304, 76);
  profile_ = lv_label_create(profileCard);
  lv_obj_set_style_text_color(profile_, lv_color_hex(themed(kText, 0x111827)), 0);
  summary_ = lv_label_create(profileCard);
  lv_obj_set_style_text_color(summary_, lv_color_hex(themed(kMuted, 0x475569)), 0);
  lv_obj_align(summary_, LV_ALIGN_BOTTOM_LEFT, 0, 0);

  lv_obj_t* detailCard = createCard(root, 8, 120, 304, 82);
  detail_ = lv_label_create(detailCard);
  lv_obj_set_width(detail_, 282);
  lv_label_set_long_mode(detail_, LV_LABEL_LONG_WRAP);
  lv_obj_set_style_text_color(detail_, lv_color_hex(themed(kMuted, 0x475569)), 0);

  footer_ = lv_obj_create(root);
  lv_obj_set_size(footer_, 304, 28);
  lv_obj_align(footer_, LV_ALIGN_BOTTOM_MID, 0, -5);
  lv_obj_set_style_bg_opa(footer_, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(footer_, 0, 0);
  lv_obj_set_style_pad_all(footer_, 0, 0);
  lv_obj_set_flex_flow(footer_, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(footer_, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  const char* labels[] = {"HOME", "MODELS", "TRENDS", "SETUP"};
  const Screen screens[] = {Screen::Home, Screen::Models, Screen::Trends, Screen::Settings};
  for (uint8_t index = 0; index < 4; ++index) {
    lv_obj_t* button = lv_button_create(footer_);
    lv_obj_set_size(button, 70, 24);
    lv_obj_set_style_radius(button, 6, 0);
    lv_obj_set_style_bg_color(button, lv_color_hex(themed(kCard, 0xFFFFFF)), 0);
    auto* action = new NavAction{this, static_cast<uint8_t>(screens[index])};
    lv_obj_set_user_data(button, action);
    lv_obj_add_event_cb(button, onNav, LV_EVENT_CLICKED, nullptr);
    lv_obj_t* label = lv_label_create(button);
    lv_label_set_text(label, labels[index]);
    lv_obj_set_style_text_color(label, lv_color_hex(themed(kBlue, 0x2563EB)), 0);
    lv_obj_center(label);
  }
  showSystemStatus("Starting services…");
}

void Dashboard::onNav(lv_event_t* event) {
  auto* action = static_cast<NavAction*>(lv_obj_get_user_data(static_cast<lv_obj_t*>(lv_event_get_target(event))));
  if (action != nullptr) action->dashboard->showScreen(static_cast<Screen>(action->screen));
}

void Dashboard::showScreen(Screen screen) {
  screen_ = screen;
  if (screen == Screen::Settings) {
    settingsRequested_ = true;
    setAccent(lv_color_hex(kAmber));
    lv_label_set_text(state_, "SETUP");
    lv_label_set_text(profile_, "Opening local setup portal");
    lv_label_set_text(summary_, "Wi-Fi configuration stays on-device");
    lv_label_set_text(detail_, "Connect to the password-protected setup network shown on this display. It will let you update Wi-Fi, profile, timezone, and refresh interval.");
    return;
  }
  if (screen == Screen::Models) {
    setAccent(lv_color_hex(kBlue));
    lv_label_set_text(state_, "MODELS");
    lv_label_set_text(profile_, hasProbe_ ? "Public model list" : "Profile check needed");
    if (hasProbe_ && lastProbe_.hasModelCount) lv_label_set_text_fmt(summary_, "Verified public total: %lu models", static_cast<unsigned long>(lastProbe_.models));
    else lv_label_set_text(summary_, "No model counters are guessed from page HTML");
    lv_label_set_text(detail_, "A model list will appear only after a documented public endpoint and field mapping are verified. This prevents incorrect downloads, prints, likes, or boosts labels.");
    return;
  }
  if (screen == Screen::Trends) {
    setAccent(lv_color_hex(kBlue));
    lv_label_set_text(state_, "TRENDS");
    lv_label_set_text(profile_, "Historical analytics");
    lv_label_set_text(summary_, "Waiting for verified numeric public fields");
    lv_label_set_text(detail_, "Trend history begins only when the provider supplies stable, semantically verified values. The device will not manufacture a graph from ambiguous counters.");
    return;
  }
  if (hasProbe_) showReady(lastConfig_, lastProbe_);
}

void Dashboard::setAccent(lv_color_t color) { lv_obj_set_style_text_color(state_, color, 0); }

void Dashboard::showProvisioning(const ProvisioningInfo& info) {
  setAccent(lv_color_hex(kAmber));
  lv_label_set_text(state_, "SETUP");
  lv_label_set_text(profile_, "Connect to device setup Wi-Fi");
  lv_label_set_text_fmt(summary_, "%s", info.ssid.c_str());
  lv_label_set_text_fmt(detail_, "Password: %s\nOpen: http://%s\nEnter your Wi-Fi and public MakerWorld profile.", info.password.c_str(), info.address.toString().c_str());
}

void Dashboard::showConnecting(const AppConfig& config, const String& ssid) {
  setAccent(lv_color_hex(kAmber));
  lv_label_set_text(state_, "CONNECTING");
  const String profile = config.profileUsername.isEmpty() ? "Profile needs setup" : String("@") + config.profileUsername;
  lv_label_set_text(profile_, profile.c_str());
  lv_label_set_text_fmt(summary_, "Wi-Fi: %s • refresh every %u min", ssid.c_str(), config.refreshMinutes);
  lv_label_set_text(detail_, "Connecting securely. Public profile data is requested directly from this device; no credentials or analytics are sent to WikiZell.");
}

void Dashboard::showReady(const AppConfig& config, const ProfileProbe& probe) {
  lastConfig_ = config;
  lastProbe_ = probe;
  hasProbe_ = true;
  if (screen_ != Screen::Home) return;
  const bool reachable = probe.state == ProfileProbeState::Reachable;
  setAccent(lv_color_hex(reachable ? kGreen : kAmber));
  lv_label_set_text(state_, reachable ? "ONLINE" : "CHECK PROFILE");
  const String profile = config.profileUsername.isEmpty() ? "No public profile configured" : String("@") + config.profileUsername;
  lv_label_set_text(profile_, profile.c_str());
  if (probe.hasFollowerCount || probe.hasFollowingCount || probe.hasModelCount) {
    String totals;
    if (probe.hasFollowerCount) totals += "Followers " + String(probe.followers);
    if (probe.hasFollowingCount) totals += (totals.isEmpty() ? "" : " • ") + String("Following ") + String(probe.following);
    if (probe.hasModelCount) totals += (totals.isEmpty() ? "" : " • ") + String("Models ") + String(probe.models);
    lv_label_set_text(summary_, totals.c_str());
  } else {
    lv_label_set_text_fmt(summary_, "Wi-Fi connected • refresh every %u min", config.refreshMinutes);
  }
  lv_label_set_text_fmt(detail_, "%s\nOnly verified public fields will be shown; unverified aggregate counters remain hidden.", probe.message.c_str());
}

void Dashboard::showSystemStatus(const String& message) {
  setAccent(lv_color_hex(kAmber));
  lv_label_set_text(state_, "STARTING");
  lv_label_set_text(profile_, "MakerWorldAnalytics");
  lv_label_set_text(summary_, "Created by Rene Girardi • WikiZell");
  lv_label_set_text(detail_, message.c_str());
}

bool Dashboard::takeSettingsRequest() {
  const bool requested = settingsRequested_;
  settingsRequested_ = false;
  return requested;
}

}  // namespace mwa
