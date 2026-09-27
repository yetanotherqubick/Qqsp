#!/usr/bin/env bash
# Check or apply clang-format over project sources.
#
# Usage: format.sh [--fix] [-- paths...] [--help]
# Honors the repository .clang-format.
#
# Exit codes: 0 formatted, 1 violations found, 2 usage error.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,10p' "$0" | sed 's/^# \{0,1\}//'
}

qqsp_require_tool clang-format

FIX=0
PATHS=()
while [ $# -gt 0 ]; do
  case "$1" in
    --fix) FIX=1; shift ;;
    -h|--help) usage; exit 0 ;;
    --) shift; PATHS=("$@"); break ;;
    *) PATHS+=("$1"); shift ;;
  esac
done

cd "$QQSP_REPO_ROOT"

if [ ${#PATHS[@]} -gt 0 ]; then
  FILES=()
  for p in "${PATHS[@]}"; do
    [ -f "$p" ] || qqsp_die_usage "not a file: $p"
    FILES+=("$p")
  done
else
  mapfile -d '' FILES < <(qqsp_app_sources)
fi
COUNT=${#FILES[@]}
[ "$COUNT" -gt 0 ] || { qqsp_ok "no source files matched"; exit 0; }

LOG="$QQSP_LOG_DIR/format.log"
mkdir -p "$QQSP_LOG_DIR"

if [ "$FIX" -eq 1 ]; then
  printf '%s\0' "${FILES[@]}" | xargs -0 clang-format -i
  qqsp_ok "formatted $COUNT file(s)"
  echo "Verify the build: tools/scripts/check.sh"
  exit 0
fi

set +e
printf '%s\0' "${FILES[@]}" | xargs -0 clang-format --dry-run --Werror >"$LOG" 2>&1
RC=$?
set -e

if [ "$RC" -eq 0 ]; then
  qqsp_ok "all $COUNT file(s) clang-formatted"
  exit 0
fi

VIOLATIONS="$(grep -cE ': error: ' "$LOG" || true)"
OFFENDERS="$(sed -n 's/^\([^ :]*\):.*/\1/p' "$LOG" | sort -u)"
OFFENDER_COUNT="$(wc -l <<<"$OFFENDERS")"
qqsp_fail "$OFFENDER_COUNT file(s) not clang-formatted ($VIOLATIONS violations)"
echo "$OFFENDERS" | head -n 50
[ "$OFFENDER_COUNT" -gt 50 ] && echo "...($((OFFENDER_COUNT - 50)) more; full log: $LOG)"
echo "Apply fixes: tools/scripts/format.sh --fix"
exit 1
