# MakerWorldAnalytics

A privacy-first, independent MakerWorld analytics dashboard for the ESP32-2432S028R Cheap Yellow Display (CYD).

> MakerWorldAnalytics collects public MakerWorld information directly from the user's ESP32. MakerWorld account credentials and analytics are not sent to WikiZell or to a WikiZell-operated server.

MakerWorldAnalytics is an independent, unofficial open-source project. It is not affiliated with, endorsed by or operated by MakerWorld or Bambu Lab.

## Status

The repository now contains the alpha foundation: a pinned PlatformIO project, standard CYD display/touch wiring, a dark LVGL 9 dashboard shell, explicit user profile configuration (no baked-in profile), and a conservative public-profile reachability check. It intentionally does **not** label or invent aggregate counters until a stable public MakerWorld endpoint and field mapping are verified.

Hardware support is currently limited to the standard single-USB ESP32-2432S028R with an ILI9341 display and XPT2046 touch controller. Physical validation is pending.

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

The firmware never contains a MakerWorld profile by default. Configure the profile from the on-device setup/settings flow when it is implemented; the input parser accepts `username`, `@username`, or a public MakerWorld profile URL.

## Privacy and data availability

The device is designed to make HTTPS requests directly to MakerWorld over the user's Wi-Fi. It does not use MakerWorld, Bambu Lab, or WikiZell credentials, cookies, a proxy, or a VPS. The verified public-page observations and current provider limitations are documented in [docs/makerworld-public-data.md](docs/makerworld-public-data.md).

## Project material

- [Implementation prompt](docs/IMPLEMENTATION_PROMPT.md)
- [Architecture](docs/architecture.md)
- [Hardware and validation](docs/hardware.md)
- [Public-data findings](docs/makerworld-public-data.md)
- [Hardware validation checklist](docs/hardware-validation-checklist.md)
- [Ko-fi](https://ko-fi.com/wikizell)

Created by René Girardi ([WikiZell](https://github.com/WikiZell)).
