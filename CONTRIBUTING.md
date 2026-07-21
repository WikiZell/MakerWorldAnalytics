# Contributing

Thank you for improving MakerWorldAnalytics. Keep changes small, reviewable, and suitable for a 4 MB classic ESP32 without PSRAM.

## Development expectations

- Run `pio run -e esp32-2432s028r`; run native tests when a host C++ compiler is available.
- Do not commit Wi-Fi credentials, cookies, MakerWorld tokens, real user data, or `setInsecure()`.
- Keep requests direct to public MakerWorld surfaces and document any endpoint/field verification in `docs/makerworld-public-data.md`.
- Do not label an observed value as downloads, prints, likes, or boosts unless its public field mapping has been verified and tested.
- Preserve responsive touch: network work belongs off the LVGL task.
- Use the project's dark theme, compact copy, and 40 px-or-larger touch targets where practical.

Open an issue before making broad hardware, provider, storage-format, or OTA changes.
