# Alpha status and validation

Last updated: 2026-07-21.

| Area | Current state | Evidence / limitation |
| --- | --- | --- |
| Repository, MIT license, issues, CI, Pages | Available | Public repository and GitHub Pages installer are live. |
| Alpha release assets | Available | `v0.1.0-alpha.1` publishes application, factory image, manifest, and SHA-256 checksums. |
| ESP32-2432S028R boot | Validated | Flashed to the CH340-connected ESP32-D0WD-V3 at `COM3`; serial boot and memory diagnostics are clean. |
| Custom provisioning and settings | Implemented | Password-protected local AP, local NVS storage, setup recovery, display and update settings. |
| BOOT recovery | Implemented | Five-second setup recovery and ten-second local reset; destructive reset not executed during hardware validation. |
| Dashboard UI | Implemented, visual validation pending | Landscape LVGL dashboard and navigation compile/boot. A serial connection cannot capture physical display pixels or touch input. |
| Public MakerWorld page | Browser verified | Public page exposes explicitly labelled Followers, Following, and 3D Models totals. Unlabelled counters are intentionally hidden. |
| Direct ESP32 MakerWorld collection | Externally blocked | Direct non-browser HTTPS receives a Cloudflare `403` challenge. The device reports this and does not bypass it or use a proxy. |
| GitHub Project board | Externally blocked | Current token has no `read:project` / `project` scope. Run `gh auth refresh -s project,read:project` to enable board creation. |

The alpha is intentionally transparent: it is installable and safe to explore, but it must not be represented as a working live MakerWorld analytics collector while direct device requests are challenged.
