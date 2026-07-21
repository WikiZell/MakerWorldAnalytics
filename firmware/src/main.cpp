#include <Arduino.h>
#include <lvgl.h>

#include "mwa/Board.h"

namespace {
constexpr uint16_t kWidth = 320;
constexpr uint16_t kHeight = 240;
constexpr uint16_t kDrawRows = 24;
lv_color_t drawBuffer[kWidth * kDrawRows];
lv_display_t* lvDisplay = nullptr;

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

lv_obj_t* card(lv_obj_t* parent, const char* title, const char* value, const char* subtitle, lv_color_t accent) {
  lv_obj_t* container = lv_obj_create(parent);
  lv_obj_set_size(container, 72, 80);
  lv_obj_set_style_radius(container, 10, 0);
  lv_obj_set_style_bg_color(container, lv_color_hex(0x171B22), 0);
  lv_obj_set_style_border_color(container, lv_color_hex(0x2D333D), 0);
  lv_obj_set_style_pad_all(container, 7, 0);
  lv_obj_t* heading = lv_label_create(container);
  lv_label_set_text(heading, title);
  lv_obj_set_style_text_color(heading, accent, 0);
  lv_obj_t* metric = lv_label_create(container);
  lv_label_set_text(metric, value);
  lv_obj_set_style_text_font(metric, LV_FONT_DEFAULT, 0);
  lv_obj_align(metric, LV_ALIGN_LEFT_MID, 0, 6);
  lv_obj_t* detail = lv_label_create(container);
  lv_label_set_text(detail, subtitle);
  lv_obj_set_style_text_color(detail, lv_color_hex(0xAAB2C0), 0);
  lv_obj_align(detail, LV_ALIGN_BOTTOM_LEFT, 0, 0);
  return container;
}

void buildDashboard() {
  lv_obj_t* root = lv_screen_active();
  lv_obj_set_style_bg_color(root, lv_color_hex(0x0B0D11), 0);
  lv_obj_set_style_text_color(root, lv_color_hex(0xF4F7FB), 0);
  lv_obj_set_style_pad_all(root, 8, 0);
  lv_obj_t* title = lv_label_create(root);
  lv_label_set_text(title, "MakerWorld");
  lv_obj_set_style_text_font(title, LV_FONT_DEFAULT, 0);
  lv_obj_align(title, LV_ALIGN_TOP_MID, -25, 0);
  lv_obj_t* accent = lv_label_create(root);
  lv_label_set_text(accent, "Analytics");
  lv_obj_set_style_text_color(accent, lv_color_hex(0x4ADE80), 0);
  lv_obj_set_style_text_font(accent, LV_FONT_DEFAULT, 0);
  lv_obj_align(accent, LV_ALIGN_TOP_MID, 62, 0);
  lv_obj_t* status = lv_label_create(root);
  lv_label_set_text(status, "SETUP REQUIRED");
  lv_obj_set_style_text_color(status, lv_color_hex(0xFBBF24), 0);
  lv_obj_align(status, LV_ALIGN_TOP_RIGHT, 0, 5);
  lv_obj_t* profile = lv_label_create(root);
  lv_label_set_text(profile, "No profile configured\nEnter a public MakerWorld username or URL in Settings.");
  lv_obj_set_style_text_color(profile, lv_color_hex(0xCDD5E1), 0);
  lv_obj_align(profile, LV_ALIGN_TOP_LEFT, 0, 35);
  lv_obj_t* row = lv_obj_create(root);
  lv_obj_set_size(row, 304, 88);
  lv_obj_align(row, LV_ALIGN_TOP_MID, 0, 82);
  lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
  lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
  lv_obj_set_style_border_width(row, 0, 0);
  lv_obj_set_style_pad_all(row, 0, 0);
  card(row, "PROFILE", "--", "Not configured", lv_color_hex(0x60A5FA));
  card(row, "MODELS", "--", "Public data only", lv_color_hex(0x4ADE80));
  card(row, "STATUS", "WAIT", "No values invented", lv_color_hex(0xFBBF24));
  card(row, "REFRESH", "15m", "When connected", lv_color_hex(0xF472B6));
  lv_obj_t* footer = lv_label_create(root);
  lv_label_set_text(footer, "HOME     MODELS     TRENDS     SETTINGS");
  lv_obj_set_style_text_color(footer, lv_color_hex(0x60A5FA), 0);
  lv_obj_align(footer, LV_ALIGN_BOTTOM_MID, 0, 0);
}
}  // namespace

void setup() {
  Serial.begin(115200);
  Serial.println("[MWA] boot: esp32-2432s028r / 0.1.0-alpha.1");
  mwa::display.init();
  mwa::display.setRotation(1);
  mwa::display.setBrightness(180);
  mwa::touch.begin();
  mwa::touch.setRotation(1);
  lv_init();
  lvDisplay = lv_display_create(kWidth, kHeight);
  lv_display_set_flush_cb(lvDisplay, flush);
  lv_display_set_buffers(lvDisplay, drawBuffer, nullptr, sizeof(drawBuffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
  lv_indev_t* input = lv_indev_create();
  lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(input, readTouch);
  buildDashboard();
  Serial.println("[MWA] ui: dashboard ready; no MakerWorld profile compiled in");
}

void loop() {
  lv_timer_handler();
  delay(5);
}
