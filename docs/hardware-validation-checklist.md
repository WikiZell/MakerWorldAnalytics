# Hardware validation checklist

- [ ] Board appears as a serial port at 115200 baud.
- [ ] Boot banner identifies `esp32-2432s028r`.
- [ ] ILI9341 renders the dark dashboard in landscape orientation.
- [ ] XPT2046 maps touch correctly after calibration.
- [ ] Wi-Fi configuration is not displayed or logged after submission.
- [ ] TLS connection rejects invalid certificates.
- [ ] Profile reachability succeeds for a known public profile.
- [ ] Recovery button flow works with unusable touch.
- [ ] Repeated refreshes leave heap stable.
- [ ] OTA update rejects a mismatched hardware manifest.
