#pragma once

#include <LovyanGFX.hpp>

namespace mwa {

class CydDisplay final : public lgfx::LGFX_Device {
 public:
  CydDisplay();

 private:
  lgfx::Bus_SPI bus_;
  lgfx::Panel_ILI9341 panel_;
  lgfx::Light_PWM light_;
  lgfx::Touch_XPT2046 touch_;
};

extern CydDisplay display;

}  // namespace mwa
