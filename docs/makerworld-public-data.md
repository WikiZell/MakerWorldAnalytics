# MakerWorld public data access

Last visually verified: 2026-07-21 against a public, logged-out profile page in the MakerWorld web UI. Direct non-browser HTTPS requests were also checked on that date.

## Verified public surface

`GET https://makerworld.com/en/@{username}` renders a public profile page over HTTPS without requiring a login for the profile inspected. The visible page exposed:

- display name and username;
- follower and following counts;
- visible model count;
- model titles and links;
- four unlabeled aggregate counters in the profile header; and
- four unlabeled counters on visible model cards.

The visible DOM did not provide a stable, documented semantic label for every aggregate value. The `Followers`, `Following`, and `3D Models (N)` labels are explicit and are the only totals now eligible for display. Firmware must not call the remaining header or model-card values downloads, prints, likes, or boosts until a repeatable endpoint and field mapping have been verified.

## Current provider contract

The alpha provider accepts a username or public profile URL, normalizes it locally, and requests the public profile page directly through TLS. It checks redirect and HTTP failure conditions, then can stream HTML without retaining the response. It extracts only the three labelled profile totals above. This avoids shipping guessed metric names or retaining a brittle page-sized response in ESP32 memory.

At the time of verification, ordinary direct HTTPS clients received a `403` Cloudflare challenge rather than the public page. A Browser could render the page, but an ESP32 must not attempt to bypass a challenge. Therefore, the current device normally reports `AccessDenied` and displays no MakerWorld totals. The streaming extraction remains deliberately dormant until a challenge-free public response can be verified end-to-end from the device.

Future endpoint research must capture, sanitize, and test a stable public JSON response before enabling metric cards. Record the request method, headers, content type, identifier, response sample, fields used, rate-limit behavior, and HTML/Cloudflare challenge behavior here. Do not log or store cookies, credentials, or user-specific responses.

## Failure handling

- `401`, `403`, `404`, `429`, and `5xx` map to explicit non-destructive status.
- HTML challenge pages are treated as a provider failure, not JSON.
- The current alpha has no cached profile metrics, so it reports the latest reachability result rather than displaying invented stale values.
- Refreshes are no more frequent than every 15 minutes. Exponential retry backoff is planned before metric collection is enabled.
