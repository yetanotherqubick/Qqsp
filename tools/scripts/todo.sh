#!/usr/bin/env bash
# Inventory TODO/FIXME/XXX/HACK markers in tracked sources.
#
# Usage: todo.sh [--all] [--since REF] [--help]
#   --all        include tools/, not just application areas
#   --since REF  limit to files added/changed since REF
#
# Output: "file:line: text" lines, then per-file counts and a total.
# Exit codes: 0 ok (markers are findings, not failures), 2 usage error.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,11p' "$0" | sed 's/^# \{0,1\}//'
}

ALL=0
SINCE=""
while [ $# -gt 0 ]; do
  case "$1" in
    --all) ALL=1; shift ;;
    --since) SINCE="${2:?--since needs a ref}"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    *) qqsp_die_usage "unknown argument: $1" ;;
  esac
done

cd "$QQSP_REPO_ROOT"

SCOPE=("${QQSP_APP_AREAS[@]}")
[ "$ALL" -eq 0 ] || SCOPE+=("tools")

PATTERN='(TODO|FIXME|XXX|HACK)'

if [ -n "$SINCE" ]; then
  FILES="$(git diff --name-only --diff-filter=ACMR "$SINCE" -- "${SCOPE[@]}")" || true
  [ -n "$FILES" ] || { qqsp_ok "no changed files in scope since $SINCE"; exit 0; }
  # shellcheck disable=SC2086
  RESULTS="$(git grep -nE "$PATTERN" -- $FILES || true)"
  LABEL="in files changed since $SINCE"
else
  RESULTS="$(git grep -nE "$PATTERN" -- "${SCOPE[@]}" || true)"
  LABEL="in scope (application areas)"
  if [ "$ALL" -eq 1 ]; then
    LABEL="in scope (application areas + tools)"
  fi
fi

qqsp_section "markers $LABEL"
if [ -z "$RESULTS" ]; then
  qqsp_ok "no markers found"
  exit 0
fi

echo "$RESULTS"
echo
echo "$RESULTS" | cut -d: -f1 | sort | uniq -c | sort -rn | sed 's/^/  /'
TOTAL="$(echo "$RESULTS" | wc -l)"
echo "total: $TOTAL marker(s)"
