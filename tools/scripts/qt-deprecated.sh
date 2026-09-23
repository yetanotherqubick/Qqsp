#!/usr/bin/env bash
# Qt deprecation ratchet: build in a scratch directory with
# QT_DISABLE_DEPRECATED_UP_TO raised to a target value, and report every API
# use that blocks it. The ratchet metric must reach 0 before the Qt 6
# migration (ROADMAP M7/M8).
#
# Usage: qt-deprecated.sh [--up-to HEX] [--keep] [--help]
#   --up-to HEX   value for QT_DISABLE_DEPRECATED_UP_TO (default 0x060000)
#   --keep        keep the scratch build directory for inspection
#
# Scratch build: build/qt-deprecated (recreated each run unless --keep).
# Exit codes: 0 no blockers, 1 blockers/build failure, 2 usage error.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,13p' "$0" | sed 's/^# \{0,1\}//'
}

UP_TO="0x060000"
KEEP=0
while [ $# -gt 0 ]; do
  case "$1" in
    --up-to) UP_TO="${2:?--up-to needs a hex value}"; shift 2 ;;
    --keep) KEEP=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) qqsp_die_usage "unknown argument: $1" ;;
  esac
done

qqsp_require_tool cmake

SCRATCH="$QQSP_BUILD_DIR/qt-deprecated"
LOG="$QQSP_LOG_DIR/qt-deprecated.log"
mkdir -p "$QQSP_LOG_DIR"

if [ "$KEEP" -eq 0 ]; then
  rm -rf "$SCRATCH"
fi
mkdir -p "$SCRATCH"

qqsp_section "configure (QT_DISABLE_DEPRECATED_UP_TO=$UP_TO)"
if ! cmake -S "$QQSP_REPO_ROOT" -B "$SCRATCH" -G Ninja -DCMAKE_BUILD_TYPE=Release \
  "-DQQSP_QT_DISABLE_DEPRECATED_UP_TO=$UP_TO" >"$LOG" 2>&1; then
  qqsp_fail "configure failed"
  tail -n 15 "$LOG"
  exit 1
fi
qqsp_ok "configure"

qqsp_section "build"
set +e
cmake --build "$SCRATCH" >>"$LOG" 2>&1
BUILD_RC=$?
set -e

ERRORS="$(qqsp_error_lines <"$LOG" | sort -u)"
ERROR_COUNT="$(wc -l <<<"$ERRORS")"
DEPRECATED_COUNT="$(grep -cE ' is deprecated' "$LOG" || true)"

if [ "$BUILD_RC" -ne 0 ]; then
  qqsp_fail "build blocked: $ERROR_COUNT unique error(s), $DEPRECATED_COUNT deprecation-related"
  qqsp_section "errors"
  if [ -n "$ERRORS" ]; then
    echo "$ERRORS" | head -n 100
    [ "$ERROR_COUNT" -gt 100 ] && echo "...($((ERROR_COUNT - 100)) more; full log: $LOG)"
  else
    tail -n 20 "$LOG"
  fi
  [ "$KEEP" -eq 1 ] || rm -rf "$SCRATCH"
  exit 1
fi

if [ "$KEEP" -eq 0 ]; then
  rm -rf "$SCRATCH"
fi
qqsp_ok "build passed with QT_DISABLE_DEPRECATED_UP_TO=$UP_TO (0 deprecation blockers)"
