#!/usr/bin/env bash
set -Eeuo pipefail
ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
BROWSER_DIR="$ROOT_DIR/browser"
for file in \
  "$BROWSER_DIR/CMakeLists.txt" "$BROWSER_DIR/main.cpp" "$BROWSER_DIR/main_window.cpp" \
  "$BROWSER_DIR/main_window.h" "$BROWSER_DIR/address_resolver.cpp" "$BROWSER_DIR/address_resolver.h" \
  "$BROWSER_DIR/tests/address_resolver_test.cpp" "$BROWSER_DIR/test-page.html" "$BROWSER_DIR/README.md"; do
  if [[ ! -s "$file" ]]; then printf 'Missing or empty standalone browser file: %s\n' "$file" >&2; exit 1; fi
done
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
printf 'BOLT standalone browser static checks passed.\n'
