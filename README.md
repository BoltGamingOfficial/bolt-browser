# BOLT Browser

**BOLT Browser 0.2** is a small Firefox-based customization prototype for a balanced mix of speed, privacy, and compatibility. It uses Mozilla Firefox's existing web engine, security updates, and extension ecosystem rather than maintaining a browser engine fork.

This repository contains a branded start page and platform-specific testing/package helpers. It is not a separate browser engine or independently maintained Firefox fork. Performance and website compatibility have not yet been benchmarked.

## Design goals
- **Balanced performance:** avoid adding background daemons or unnecessary runtime dependencies.
- **Standard privacy:** keep Firefox's normal protections and avoid overly aggressive defaults that commonly break sites.
- **Compatibility:** preserve Firefox's web platform, extensions, logins, downloads, and media support.
- **User control:** homepage is not locked; system policy changes are opt-in.
- **Maintainability:** rely on Firefox security updates instead of carrying a browser-engine fork.

Searches from the start page go to DuckDuckGo. This does not make searches anonymous. Firefox's own Enhanced Tracking Protection settings remain separate from telemetry and enterprise policy settings.

## Windows 11 testing
For the current development phase, Windows users can test the start page using their installed Firefox without changing existing policies or profile settings. See [`windows/README.md`](windows/README.md) and run `windows/launch-bolt-browser.ps1` from a PowerShell session.

## Current features
- Gold-and-black local start page with search and gaming/project shortcuts.
- Firefox policy template disabling Firefox Studies, Pocket, and Firefox telemetry.
- Windows PowerShell launcher for a non-invasive start-page smoke test.
- Arch PKGBUILD that installs the start page, an example policy file, and an opt-in policy merge helper.
- Offline static checks and a GitHub Actions quality workflow.

## Repository layout
- `startpage/index.html` — static local start page.
- `firefox/policies.json` — Firefox enterprise policy template for Linux packaging.
- `windows/launch-bolt-browser.ps1` — Windows smoke-test launcher; does not change Firefox settings.
- `windows/README.md` — Windows setup and test notes.
- `PKGBUILD` — Arch package definition.
- `packaging/enable-firefox-policy.sh` — explicit, backup-preserving policy merge helper.
- `tests/check-browser.sh` — offline static checks.
- `.github/workflows/quality.yml` — CI checks.

## Build and install on Arch Linux
From a checkout of this repository, inspect the PKGBUILD and build the package:
```sh
makepkg --printsrcinfo
makepkg -s
```

The package installs `/usr/share/bolt-browser/startpage.html`, `/usr/share/bolt-browser/policies.json.example`, and `/usr/bin/bolt-browser-enable-firefox-policy`. It does not automatically modify Firefox's active policy file.

### Optional Firefox policy integration
If you want the supplied defaults applied system-wide, run:
```sh
sudo bolt-browser-enable-firefox-policy
```

The helper asks before merging into an existing policy file, preserves unrelated policy keys, creates a timestamped backup when a target already exists, and writes the result atomically. It refuses malformed policy JSON rather than overwriting it. To target a non-default location, pass `--target /absolute/path/to/policies.json`.

The default target is `/usr/lib/firefox/distribution/policies.json`. Verify the active result in Firefox at `about:policies`. If the distribution manages policies elsewhere, do not run the helper until you have confirmed the correct path.

## Validate locally
Linux static checks require Bash and Python 3:
```sh
bash tests/check-browser.sh
python3 -m json.tool firefox/policies.json
bash -n packaging/enable-firefox-policy.sh
```

The repository CI validates source structure and shell syntax. These checks do not prove that an Arch package builds, Firefox accepts the policies, websites work correctly, or performance improves.

## Privacy and compatibility
- Private windows should be tested against Firefox's native behavior; this project does not replace Firefox's private-browsing implementation.
- Telemetry policy settings are not the same as tracking protection or anonymity.
- Standard Firefox protections are preferred over aggressive blocking by default.
- No AI features, background agents, or new browser daemons are included.

## Roadmap
1. **0.1 — Prototype:** branded start page, policy template, static validation.
2. **0.2 — Packaging:** Arch package and opt-in policy merge helper.
3. **0.3 — Windows smoke test:** launch the start page in Firefox without modifying the user's profile.
4. **0.4 — Runtime validation:** build the package on Arch, test Firefox `about:policies`, private windows, extensions, logins, downloads, video, and common gaming websites.
5. **0.5 — Performance baseline:** record startup time, idle RAM/CPU, and multi-tab behavior under documented conditions before tuning defaults.

Do not treat performance, privacy, or compatibility as verified until measured with documented test conditions.
