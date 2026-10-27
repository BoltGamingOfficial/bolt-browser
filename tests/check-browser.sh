#!/usr/bin/env bash
set -Eeuo pipefail
ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
POLICIES="$ROOT_DIR/firefox/policies.json"
STARTPAGE="$ROOT_DIR/startpage/index.html"
PKGBUILD="$ROOT_DIR/PKGBUILD"
POLICY_HELPER="$ROOT_DIR/packaging/enable-firefox-policy.sh"
for file in "$POLICIES" "$STARTPAGE" "$PKGBUILD" "$POLICY_HELPER"; do
  if [[ ! -s "$file" ]]; then printf 'Missing or empty BOLT Browser file: %s\n' "$file" >&2; exit 1; fi
done
python3 - "$POLICIES" "$PKGBUILD" <<'PY'
import json, pathlib, sys
policy_path, pkgbuild_path = map(pathlib.Path, sys.argv[1:])
document = json.loads(policy_path.read_text(encoding='utf-8'))
policies = document.get('policies', {})
for key in ('DisableFirefoxStudies', 'DisablePocket', 'DisableTelemetry'):
    if policies.get(key) is not True: raise SystemExit(f'Firefox policy {key} must be true')
homepage = policies.get('Homepage', {})
if homepage.get('URL') != 'file:///usr/share/bolt-browser/startpage.html': raise SystemExit('Homepage URL mismatch')
if homepage.get('StartPage') != 'homepage': raise SystemExit('Homepage StartPage policy must be homepage')
if homepage.get('Locked') is not False: raise SystemExit('Homepage must remain user-changeable')
pkgbuild = pkgbuild_path.read_text(encoding='utf-8')
for expected in ('pkgname=bolt-browser', 'startpage.html', 'policies.json.example', 'bolt-browser-enable-firefox-policy'):
    if expected not in pkgbuild: raise SystemExit(f'PKGBUILD missing packaging rule: {expected}')
if '/usr/lib/firefox/distribution/policies.json' in pkgbuild: raise SystemExit('PKGBUILD must not overwrite system Firefox policies automatically')
PY
grep -Fq '<title>BOLT Browser</title>' "$STARTPAGE"
grep -Fq 'action="https://duckduckgo.com/"' "$STARTPAGE"
grep -Fq 'name="q"' "$STARTPAGE"
grep -Fq 'https://github.com/BoltGamingOfficial/bolt-os' "$STARTPAGE"
grep -Fq 'https://store.steampowered.com/' "$STARTPAGE"
grep -Fq 'Merge BOLT Browser policies into a Firefox policies file after confirmation.' "$POLICY_HELPER"
bash -n "$POLICY_HELPER"
printf 'BOLT Browser static checks passed.\n'
