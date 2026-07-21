#include "mwa/Board.h"

namespace mwa {
namespace {
constexpr int kTftCs = 15;
constexpr int kTftDc = 2;
constexpr int kTftBacklight = 21;
constexpr int kSpiSclk = 14;
constexpr int kSpiMosi = 13;
constexpr int kSpiMiso = 12;
constexpr int kTouchCs = 33;
constexpr int kTouchIrq = 36;
constexpr int kTouchSclk = 25;
constexpr int kTouchMosi = 32;
constexpr int kTouchMiso = 39;
}  // namespace

CydDisplay::CydDisplay() {
    auto busConfig = bus_.config();
    busConfig.spi_host = VSPI_HOST;
    busConfig.spi_mode = 0;
    busConfig.freq_write = 40000000;
    busConfig.freq_read = 16000000;
    busConfig.pin_sclk = kSpiSclk;
    busConfig.pin_mosi = kSpiMosi;
    busConfig.pin_miso = kSpiMiso;
    busConfig.pin_dc = kTftDc;
    bus_.config(busConfig);
    panel_.setBus(&bus_);
    auto panelConfig = panel_.config();
    panelConfig.pin_cs = kTftCs;
    panelConfig.pin_rst = -1;
    panelConfig.panel_width = 240;
    panelConfig.panel_height = 320;
    panelConfig.memory_width = 240;
    panelConfig.memory_height = 320;
    panel_.config(panelConfig);
    auto lightConfig = light_.config();
    lightConfig.pin_bl = kTftBacklight;
    lightConfig.invert = false;
    lightConfig.freq = 44100;
    light_.config(lightConfig);
    panel_.setLight(&light_);

    auto touchConfig = touch_.config();
    touchConfig.spi_host = HSPI_HOST;
    touchConfig.freq = 1000000;
    touchConfig.pin_sclk = kTouchSclk;
    touchConfig.pin_mosi = kTouchMosi;
    touchConfig.pin_miso = kTouchMiso;
    touchConfig.pin_cs = kTouchCs;
    touchConfig.pin_int = kTouchIrq;
    touchConfig.bus_shared = false;
    touchConfig.x_min = 0;
    touchConfig.x_max = 4095;
    touchConfig.y_min = 0;
    touchConfig.y_max = 4095;
    touch_.config(touchConfig);
    panel_.setTouch(&touch_);
    setPanel(&panel_);
}
CydDisplay display;

}  // namespace mwa
