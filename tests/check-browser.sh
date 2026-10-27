#!/usr/bin/env bash
set -Eeuo pipefail
ROOT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
POLICIES="$ROOT_DIR/firefox/policies.json"
STARTPAGE="$ROOT_DIR/startpage/index.html"
for file in "$POLICIES" "$STARTPAGE"; do
  if [[ ! -s "$file" ]]; then
    printf 'Missing or empty BOLT Browser file: %s\n' "$file" >&2
    exit 1
  fi
done
python3 - "$POLICIES" <<'PY'
import json
import sys
with open(sys.argv[1], encoding="utf-8") as handle:
    document = json.load(handle)
policies = document.get("policies", {})
for key in ("DisableFirefoxStudies", "DisablePocket", "DisableTelemetry"):
    if policies.get(key) is not True:
        raise SystemExit(f"Firefox policy {key} must be true")
homepage = policies.get("Homepage", {})
if homepage.get("URL") != "file:///usr/share/bolt-browser/startpage.html":
    raise SystemExit("Homepage URL does not match the planned installed start page")
if homepage.get("StartPage") != "homepage":
    raise SystemExit("Homepage StartPage policy must be homepage")
if homepage.get("Locked") is not False:
    raise SystemExit("Homepage must remain user-changeable")
PY
grep -Fq '<title>BOLT Browser</title>' "$STARTPAGE"
grep -Fq 'action="https://duckduckgo.com/"' "$STARTPAGE"
grep -Fq 'name="q"' "$STARTPAGE"
grep -Fq 'https://github.com/BoltGamingOfficial/bolt-os' "$STARTPAGE"
grep -Fq 'https://store.steampowered.com/' "$STARTPAGE"
printf 'BOLT Browser static checks passed.\n'
