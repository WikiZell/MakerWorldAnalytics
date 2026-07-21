# Hardware

Primary target: ESP32-2432S028R (single USB), ESP32-WROOM-32, 4 MB flash, ILI9341 and XPT2046.

The initial pin profile uses VSPI: TFT SCLK 14, MOSI 13, MISO 12, CS 15, DC 2, backlight 21; touch CS 33 and IRQ 36. Variants must be added as separate PlatformIO environments, not auto-detected at runtime.

The partition table reserves NVS, OTA metadata, two 1.5 MiB OTA application slots, and LittleFS.
