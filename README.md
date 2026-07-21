# MakerWorldAnalytics

A privacy-first, independent MakerWorld analytics dashboard for the ESP32-2432S028R Cheap Yellow Display (CYD).

> MakerWorldAnalytics collects public MakerWorld information directly from the user's ESP32. MakerWorld account credentials and analytics are not sent to WikiZell or to a WikiZell-operated server.

MakerWorldAnalytics is an independent, unofficial open-source project. It is not affiliated with, endorsed by or operated by MakerWorld or Bambu Lab.

## Status

The repository now contains the alpha foundation: a pinned PlatformIO project, standard CYD display/touch wiring, a touch-driven LVGL 9 dashboard, protected local configuration (no baked-in profile), runtime diagnostics, recovery/factory-reset flow, GitHub release assets, and a web installer. It intentionally does **not** label or invent aggregate counters.

At present, MakerWorld returns a Cloudflare challenge to ordinary direct HTTPS clients. The device reports this clearly and does not bypass it, proxy requests, or display fabricated values. See [public-data findings](docs/makerworld-public-data.md) before expecting live profile totals.

Hardware support is currently limited to the standard single-USB ESP32-2432S028R with an ILI9341 display and XPT2046 touch controller. The initial boot and serial output have been validated; visual/touch validation is still pending.

## Build

Install PlatformIO, then run:

```sh
cd firmware
pio run -e esp32-2432s028r
pio test -e native
```

To upload after a serial device appears:

```sh
pio run -e esp32-2432s028r -t upload
pio device monitor -b 115200
```

The firmware never contains a MakerWorld profile by default. On first boot (or while holding **BOOT** during startup), it creates a password-protected setup Wi-Fi network. Enter the home Wi-Fi and profile there; the input parser accepts `username`, `@username`, or a public MakerWorld profile URL.

## Privacy and data availability

The device is designed to make HTTPS requests directly to MakerWorld over the user's Wi-Fi. It does not use MakerWorld, Bambu Lab, or WikiZell credentials, cookies, a proxy, or a VPS. The verified public-page observations and current provider limitations are documented in [docs/makerworld-public-data.md](docs/makerworld-public-data.md).

The current installer is available at [wikizell.github.io/MakerWorldAnalytics](https://wikizell.github.io/MakerWorldAnalytics/); it installs the `v0.1.0-alpha.1` prerelease for the standard single-USB CYD only.

## Project material

- [Implementation summary](docs/IMPLEMENTATION_PROMPT.md) and [complete supplied brief](docs/ORIGINAL_PROJECT_BRIEF.txt)
- [Architecture](docs/architecture.md)
- [Hardware and validation](docs/hardware.md)
- [Public-data findings](docs/makerworld-public-data.md)
- [Hardware validation checklist](docs/hardware-validation-checklist.md)
- [Alpha status and validation](docs/alpha-status.md)
- [First-time setup](docs/first-setup.md)
- [Privacy and security](docs/privacy-and-security.md)
- [Web installer](docs/web-installer.md)
- [Ko-fi](https://ko-fi.com/wikizell)

Created by René Girardi ([WikiZell](https://github.com/WikiZell)).
