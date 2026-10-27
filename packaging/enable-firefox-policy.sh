#!/usr/bin/env bash
set -Eeuo pipefail

readonly template='/usr/share/bolt-browser/policies.json.example'
target='/usr/lib/firefox/distribution/policies.json'

usage() {
  printf 'Usage: %s [--target PATH]\n' "${0##*/}"
  printf 'Merge BOLT Browser policies into a Firefox policies file after confirmation.\n'
}

while (($#)); do
  case "$1" in
    --target)
      if (($# < 2)) || [[ -z "$2" ]]; then usage >&2; exit 2; fi
      target=$2; shift 2 ;;
    --help|-h) usage; exit 0 ;;
    *) usage >&2; exit 2 ;;
  esac
done

if [[ $EUID -ne 0 ]]; then
  printf 'Error: run this helper with sudo to write system Firefox policies.\n' >&2
  exit 1
fi
if [[ ! -r $template ]]; then
  printf 'Error: policy template not found: %s\n' "$template" >&2
  exit 1
fi
if [[ $target != /* ]]; then
  printf 'Error: target must be an absolute path.\n' >&2
  exit 2
fi

if [[ -e $target ]]; then
  printf 'This will merge BOLT settings into the existing policy file: %s\n' "$target"
  printf 'BOLT keys with the same names will replace their existing values.\n'
  read -r -p 'Create a backup and continue? [y/N] ' answer
  case "$answer" in y|Y|yes|YES) ;; *) printf 'Cancelled; no changes made.\n'; exit 0 ;; esac
fi
python3 - "$template" "$target" <<'PY'
import json
import os
import shutil
import sys
import tempfile
from datetime import datetime
from pathlib import Path

template_path, target_arg = sys.argv[1:]
template_path, target = Path(template_path), Path(target_arg)
try:
    template_doc = json.loads(template_path.read_text(encoding='utf-8'))
    bolt_policies = template_doc['policies']
    if not isinstance(bolt_policies, dict):
        raise ValueError("template 'policies' must be an object")
    exists = target.exists()
    current_doc = {}
    if exists:
        current_doc = json.loads(target.read_text(encoding='utf-8'))
        if not isinstance(current_doc, dict):
            raise ValueError('existing policy file root must be an object')
        current_policies = current_doc.get('policies', {})
        if not isinstance(current_policies, dict):
            raise ValueError("existing 'policies' must be an object")
    else:
        current_policies = {}
    merged_doc = dict(current_doc)
    merged_doc['policies'] = {**current_policies, **bolt_policies}
    serialized = json.dumps(merged_doc, indent=2, ensure_ascii=False) + '\n'
except (OSError, json.JSONDecodeError, KeyError, ValueError) as exc:
    print(f'Error: cannot safely merge policy files: {exc}', file=sys.stderr)
    raise SystemExit(1)

target.parent.mkdir(parents=True, exist_ok=True)
if exists:
    stamp = datetime.now().strftime('%Y%m%d%H%M%S')
    backup = target.with_name(f'{target.name}.bak.{stamp}')
    suffix = 1
    while backup.exists():
        backup = target.with_name(f'{target.name}.bak.{stamp}.{suffix}')
        suffix += 1
    shutil.copy2(target, backup)
    print(f'Backup created: {backup}')
mode = (target.stat().st_mode & 0o777) if exists else 0o644
fd, temporary_name = tempfile.mkstemp(prefix=f'.{target.name}.', dir=target.parent)
try:
    with os.fdopen(fd, 'w', encoding='utf-8') as handle:
        handle.write(serialized)
        handle.flush()
        os.fsync(handle.fileno())
    os.chmod(temporary_name, mode)
    os.replace(temporary_name, target)
except Exception:
    try: os.unlink(temporary_name)
    except FileNotFoundError: pass
    raise
print(f'Merged BOLT Browser policies into: {target}')
print('Review the result in Firefox at about:policies, then restart Firefox.')
PY
