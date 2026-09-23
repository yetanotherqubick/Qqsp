#!/usr/bin/env bash
# Run the project's deterministic check: configure + build with the CMake
# preset (+ CTest automatically once tests exist), with warning capture and
# baseline comparison.
#
# Usage: check.sh [--preset NAME] [--clean] [--no-configure] [--no-test]
#                 [--record-baseline | --compare-baseline] [--help]
#
# Modes:
#   (default)            configure + build (+ test); summary line
#   --record-baseline    clean build; store sorted unique warnings under
#                        build/baseline/warnings.txt
#   --compare-baseline   clean build; print ONLY warnings not present in the
#                        stored baseline (new warnings = regressions); exit 1
#                        if any
#
# Exit codes: 0 ok, 1 failure/new warnings, 2 usage error.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'
}

PRESET="$QQSP_PRESET_DEFAULT"
CLEAN=0
NO_CONFIGURE=0
NO_TEST=0
MODE="build"

while [ $# -gt 0 ]; do
  case "$1" in
    --preset) PRESET="${2:?--preset needs a value}"; shift 2 ;;
    --clean) CLEAN=1; shift ;;
    --no-configure) NO_CONFIGURE=1; shift ;;
    --no-test) NO_TEST=1; shift ;;
    --record-baseline)
      [ "$MODE" = "build" ] || qqsp_die_usage "--record-baseline and --compare-baseline are exclusive"
      MODE="record"; shift ;;
    --compare-baseline)
      [ "$MODE" = "build" ] || qqsp_die_usage "--record-baseline and --compare-baseline are exclusive"
      MODE="compare"; shift ;;
    -h|--help) usage; exit 0 ;;
    *) qqsp_die_usage "unknown argument: $1" ;;
  esac
done

[ -f "$QQSP_REPO_ROOT/CMakePresets.json" ] || qqsp_die_usage "CMakePresets.json not found; run from the repository"

LOG="$QQSP_LOG_DIR/check.log"
BASELINE_DIR="$QQSP_BUILD_DIR/baseline"
BASELINE="$BASELINE_DIR/warnings.txt"
mkdir -p "$QQSP_LOG_DIR"

if [ "$MODE" != "build" ] || [ "$CLEAN" -eq 1 ]; then
  BUILD_ARGS=(--clean-first)
else
  BUILD_ARGS=()
fi

# --- configure ---------------------------------------------------------------

if [ "$NO_CONFIGURE" -eq 0 ]; then
  if ! cmake --preset "$PRESET" >"$LOG" 2>&1; then
    qqsp_fail "configure failed ($PRESET); last output:"
    tail -n 15 "$LOG"
    exit 1
  fi
  qqsp_ok "configure ($PRESET)"
fi

# --- build -------------------------------------------------------------------

if ! cmake --build --preset "$PRESET" "${BUILD_ARGS[@]}" >>"$LOG" 2>&1; then
  qqsp_fail "build failed"
  qqsp_section "errors"
  qqsp_error_lines <"$LOG" | sort -u | head -n 40
  echo "...(full log: $LOG)"
  exit 1
fi

WARNINGS="$(grep -cE ': warning: ' "$LOG" || true)"

# --- tests (present only after M5 adds CTest) --------------------------------

TEST_NOTE=""
if [ "$NO_TEST" -eq 0 ] && [ -f "$QQSP_BUILD_DIR/CTestTestfile.cmake" ]; then
  TEST_LOG="$QQSP_LOG_DIR/ctest.log"
  if ctest --test-dir "$QQSP_BUILD_DIR" --output-on-failure >"$TEST_LOG" 2>&1; then
    TEST_NOTE=", ctest passed"
  else
    qqsp_fail "ctest failed"
    tail -n 40 "$TEST_LOG"
    exit 1
  fi
fi

# --- baseline modes ----------------------------------------------------------

case "$MODE" in
  record)
    mkdir -p "$BASELINE_DIR"
    qqsp_warning_lines <"$LOG" | sort -u >"$BASELINE"
    COUNT="$(wc -l <"$BASELINE")"
    qqsp_ok "baseline recorded: $BASELINE ($COUNT unique warnings)"
    echo "Record the toolchain alongside it: tools/scripts/env-info.sh --markdown"
    ;;
  compare)
    [ -f "$BASELINE" ] || qqsp_die_usage "no baseline found at $BASELINE; run: check.sh --record-baseline"
    NEW="$QQSP_LOG_DIR/warnings-new.txt"
    qqsp_warning_lines <"$LOG" | sort -u >"$QQSP_LOG_DIR/warnings-now.txt"
    comm -13 "$BASELINE" "$QQSP_LOG_DIR/warnings-now.txt" >"$NEW" || true
    NEW_COUNT="$(wc -l <"$NEW")"
    if [ "$NEW_COUNT" -gt 0 ]; then
      qqsp_fail "$NEW_COUNT new warning(s) vs baseline"
      head -n 100 "$NEW"
      [ "$NEW_COUNT" -gt 100 ] && echo "...($((NEW_COUNT - 100)) more; full list: $NEW)"
      exit 1
    fi
    qqsp_ok "no new warnings vs baseline ($WARNINGS pre-existing)"
    ;;
  *)
    qqsp_ok "check passed: 0 errors, $WARNINGS warnings${TEST_NOTE}"
    ;;
esac
