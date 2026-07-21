# Privacy and security design

All normal MakerWorld requests originate on the user's ESP32. The project does not use a WikiZell server, a centralized proxy, account credentials, cookies, or private Creator Center APIs.

The alpha public-profile probe validates the Google Trust Services Root R4 chain used by `makerworld.com`; it does not use `setInsecure()`. The root was obtained from the official [Google Trust Services repository](https://i.pki.goog/r4.pem) and verified against the live site on 2026-07-21. Future providers must retain explicit certificate validation and response-size/content-type checks.
