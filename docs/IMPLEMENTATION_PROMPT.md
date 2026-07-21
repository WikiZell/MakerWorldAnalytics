# MakerWorldAnalytics implementation prompt

This repository was initiated from the project brief supplied on 2026-07-21. Keep this file with future design/review material.

Build an MIT-licensed, public `WikiZell/MakerWorldAnalytics` project for the ESP32-2432S028R Cheap Yellow Display: Arduino/C++17, PlatformIO, LVGL 9, LovyanGFX, XPT2046 touch, Preferences/NVS, LittleFS, and direct HTTPS access to **public** MakerWorld information. It must never use MakerWorld credentials, cookies, private Creator Center APIs, a WikiZell server, or a third-party analytics service. Never hardcode Wi-Fi credentials, a test profile, secrets, or `setInsecure()`.

The first board is a 320x240 landscape ILI9341/XPT2046 CYD without PSRAM. Use explicit compile-time board profiles, partial DMA-capable LVGL buffers, a modular architecture, and keep network work outside the LVGL task. Build a modern dark, high-contrast, touch-friendly dashboard inspired by the supplied reference, but show only data that has been reliably verified as public. Missing capabilities must be represented honestly rather than fabricated.

The intended product includes a fully custom captive Wi-Fi portal, setup wizard, local settings portal, touch calibration, recovery flow, persistent local history/deltas, dashboard/models/trends/settings/status/credits screens, GitHub OTA manifests, GitHub Actions, GitHub Pages ESP Web Tools installer, unit tests, documentation, and alpha release automation. Destructive actions require confirmation; factory reset removes configuration/history but preserves firmware. Document uncertainty in MakerWorld behavior and implement clear failure states.

Use `https://makerworld.com/en/@WikiZell` only as a live verification target during development. It must not be stored in source, firmware defaults, fixtures, or release configuration.

Creator: René Girardi / WikiZell. Support: https://ko-fi.com/wikizell. Include the independent-project disclaimer in user-facing documentation and credits.
