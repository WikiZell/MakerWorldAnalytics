# OTA updates

The OTA layout contains two 1.5 MiB application slots and a 960 KiB LittleFS partition. Release automation produces a hardware-specific manifest containing a version, binary URL, size, SHA-256, channel, and target identifier.

Firmware must validate TLS, the manifest schema, semantic version, hardware target, expected size, and SHA-256 before switching boot slots. A failed update check must not prevent cached dashboard data from being displayed.

OTA installation UI and rollback validation remain alpha work; no automatic update is performed by the current firmware.
