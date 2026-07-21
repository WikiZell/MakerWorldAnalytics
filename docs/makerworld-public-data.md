# MakerWorld public data access

Last visually verified: 2026-07-21 against a public, logged-out profile page in the MakerWorld web UI.

## Verified public surface

`GET https://makerworld.com/en/@{username}` renders a public profile page over HTTPS without requiring a login for the profile inspected. The visible page exposed:

- display name and username;
- follower and following counts;
- visible model count;
- model titles and links;
- four unlabeled aggregate counters in the profile header; and
- four unlabeled counters on visible model cards.

The visible DOM did not provide a stable, documented semantic label for every aggregate value. Consequently, firmware must not call those values downloads, prints, likes, or boosts until a repeatable endpoint and field mapping have been verified.

## Current provider contract

The alpha provider accepts a username or public profile URL, normalizes it locally, and requests the public profile page directly through TLS. It checks redirect and HTTP failure conditions and reports reachability. It deliberately does not scrape presentation HTML for totals. This avoids shipping guessed metric names or a brittle undocumented parser.

Future endpoint research must capture, sanitize, and test a stable public JSON response before enabling metric cards. Record the request method, headers, content type, identifier, response sample, fields used, rate-limit behavior, and HTML/Cloudflare challenge behavior here. Do not log or store cookies, credentials, or user-specific responses.

## Failure handling

- `401`, `403`, `404`, `429`, and `5xx` map to explicit non-destructive status.
- HTML challenge pages are treated as a provider failure, not JSON.
- Cached values remain visible but are marked stale.
- Refreshes are no more frequent than every 15 minutes and use backoff after failures.
