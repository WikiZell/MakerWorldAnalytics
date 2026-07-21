# Architecture

The firmware separates board I/O, the LVGL lifecycle, profile-input normalization, and MakerWorld access. UI state is capability-based: a card is only enabled when the provider explicitly supplies its data. This lets a later verified JSON provider replace the alpha reachability provider without rewriting screens or historical analytics.

The first target uses a 320x240 landscape LVGL partial draw buffer; it does not allocate a full-screen framebuffer. Settings are deliberately empty on first boot—no test profile is compiled into firmware.
