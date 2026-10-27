# BOLT Browser

**BOLT Browser 0.1** is a small Firefox-based customization prototype for BOLT OS. It keeps Mozilla Firefox's existing web engine, security updates, extension ecosystem, and compatibility, while adding BOLT branding and a few user-respecting defaults.

This repository currently contains configuration and a local start page; it is **not** a separate browser binary, Firefox fork, or installable Arch package yet.

## MVP

- Gold-and-black BOLT start page with search and gaming/project shortcuts.
- Firefox enterprise policies disable Firefox Studies, Pocket, and Firefox telemetry.
- The BOLT homepage is a default, not locked; users can change it.
- No new daemon, account, AI feature, or browser runtime.

Searches are sent to DuckDuckGo. This does not make searches anonymous. Firefox's built-in tracking protection remains available and should be reviewed separately from telemetry settings.

## Repository layout

- `startpage/index.html` — static local start page.
- `firefox/policies.json` — Firefox enterprise policy template.
- `tests/check-browser.sh` — offline static checks.
- `.github/workflows/quality.yml` — runs the checks on pushes and pull requests.

## Validate locally

Requirements: Bash and Python 3.

```sh
bash tests/check-browser.sh
python3 -m json.tool firefox/policies.json
```

These checks validate the source files; they do not prove Firefox integration, ISO bootability, or website compatibility.

## Prototype integration on Arch Linux

The intended system-wide destinations are:

- Start page: `/usr/share/bolt-browser/startpage.html`
- Firefox policies: `/usr/lib/firefox/distribution/policies.json`

The policy's homepage points to the installed start page. **Do not overwrite an existing policies file blindly**: Firefox policies are commonly managed by the distribution or administrator. Merge the BOLT policy keys with any existing policy file before deployment.

After integrating into a BOLT OS image, launch Firefox and verify `about:policies`, change the homepage in settings, and test websites, video, downloads, extensions, and private browsing on the target ISO.

## Roadmap

1. **0.1 — Prototype:** branded start page, policy template, static validation.
2. **0.2 — Arch integration:** package the assets and policies safely, preserving existing policy configuration.
3. **0.3 — Validation:** test on a built BOLT OS ISO and document actual compatibility/performance results.
4. **Later:** decide whether a Firefox theme, icon, or managed preferences add value.

No performance or compatibility claims should be considered verified until measured on actual hardware or a documented VM.
