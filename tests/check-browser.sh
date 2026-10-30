#!/usr/bin/env bash
set -Eeuo pipefail
ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
POLICIES="$ROOT_DIR/firefox/policies.json"
STARTPAGE="$ROOT_DIR/startpage/index.html"
PKGBUILD="$ROOT_DIR/PKGBUILD"
POLICY_HELPER="$ROOT_DIR/packaging/enable-firefox-policy.sh"
WINDOWS_LAUNCHER="$ROOT_DIR/windows/launch-bolt-browser.ps1"
WINDOWS_README="$ROOT_DIR/windows/README.md"
BROWSER_DIR="$ROOT_DIR/browser"
for file in "$POLICIES" "$STARTPAGE" "$PKGBUILD" "$POLICY_HELPER" "$WINDOWS_LAUNCHER" "$WINDOWS_README" \
  "$BROWSER_DIR/CMakeLists.txt" "$BROWSER_DIR/main.cpp" "$BROWSER_DIR/main_window.cpp" \
  "$BROWSER_DIR/main_window.h" "$BROWSER_DIR/address_resolver.cpp" "$BROWSER_DIR/address_resolver.h" \
  "$BROWSER_DIR/tests/address_resolver_test.cpp" "$BROWSER_DIR/test-page.html" "$BROWSER_DIR/README.md"; do
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
grep -Fq 'Start-Process -FilePath $firefox' "$WINDOWS_LAUNCHER"
grep -Fq 'does not' "$WINDOWS_README"
grep -Fq 'Qt6::WebEngineWidgets' "$BROWSER_DIR/CMakeLists.txt"
grep -Fq 'BOLT Browser is running' "$BROWSER_DIR/test-page.html"
grep -Fq 'error.rejectCertificate();' "$BROWSER_DIR/main_window.cpp"
grep -Fq 'setPersistentStoragePath' "$BROWSER_DIR/main_window.cpp"
grep -Fq 'qt_generate_deploy_app_script' "$BROWSER_DIR/CMakeLists.txt"
grep -Fq 'localhost:3000' "$BROWSER_DIR/tests/address_resolver_test.cpp"
grep -Fq '2001:db8::1' "$BROWSER_DIR/tests/address_resolver_test.cpp"
grep -Fq 'certificateRejected_' "$BROWSER_DIR/main_window.cpp"
if grep -REn 'acceptCertificate|ignoreCertificateErrors|QTWEBENGINE_DISABLE_SANDBOX|--no-sandbox' "$BROWSER_DIR" --exclude=README.md; then
  printf 'Unsafe WebEngine TLS or sandbox override found.\n' >&2
  exit 1
fi
bash -n "$POLICY_HELPER"
printf 'BOLT Browser static checks passed.\n'
