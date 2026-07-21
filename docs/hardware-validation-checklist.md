# Hardware validation checklist

## Recorded alpha session — 2026-07-21

The alpha image was uploaded to a CH340-connected ESP32-D0WD-V3 on `COM3`. Esptool verified the write hash and the board booted from flash, emitting the MakerWorldAnalytics boot banner and setup-portal address (`192.168.4.1`) over serial. Display pixels, touch mapping, Wi-Fi, and a GUI screenshot were not independently observable through the serial connection and remain pending.

- [x] Board appears as a serial port at 115200 baud.
- [x] Firmware uploads and boots from flash.
- [x] Boot banner identifies `esp32-2432s028r`.
- [ ] ILI9341 renders the dark dashboard in landscape orientation.
- [ ] XPT2046 maps touch correctly after calibration.
- [ ] Wi-Fi configuration is not displayed or logged after submission.
- [ ] TLS connection rejects invalid certificates.
- [ ] Profile reachability succeeds for a known public profile.
- [ ] Recovery button flow works with unusable touch.
- [ ] Repeated refreshes leave heap stable.
- [ ] OTA update rejects a mismatched hardware manifest.
