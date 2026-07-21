#pragma once

#include <LovyanGFX.hpp>
#include <XPT2046_Touchscreen.h>

namespace mwa {

class CydDisplay final : public lgfx::LGFX_Device {
 public:
  CydDisplay();

 private:
  lgfx::Bus_SPI bus_;
  lgfx::Panel_ILI9341 panel_;
  lgfx::Light_PWM light_;
};

extern CydDisplay display;
extern XPT2046_Touchscreen touch;

}  // namespace mwa
