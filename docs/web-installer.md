# Web installer

The GitHub Pages installer targets only the standard single-USB ESP32-2432S028R. Use Chrome or Edge with a USB data cable. Release automation publishes a versioned ESP Web Tools manifest only after the bootloader, partition table, OTA metadata, application binary, and merged factory-flash image are built. Releases include checksums for every flashable binary.

Do not use the installer for two-USB, ST7789, ILI9342, or other CYD variants until a dedicated verified build environment is released.
