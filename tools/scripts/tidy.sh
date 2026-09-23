#!/usr/bin/env bash
# Run clang-tidy over application sources using the build's compile database.
#
# Usage: tidy.sh [--since REF] [--fix] [--no-fail] [--jobs N] [--with-engine]
#                [--checks SPEC] [-- paths...] [--help]
#
#   --since REF    limit to files added/changed since REF (git diff, A/C/M/R)
#   --fix          apply suggested fixes (review with git diff + check.sh!)
#   --no-fail      report findings without failing (exit 0)
#   --with-engine  include engine C files with a minimal C-oriented check set
#   --checks SPEC  append check configuration (clang-tidy --checks syntax)
#
# Scope follows build/compile_commands.json: enabled sources only, generated
# files excluded, engine tree opt-in. Findings are a worklist, not a gate.
#
# Exit codes: 0 clean (or --no-fail), 1 findings, 2 usage/missing tool.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,20p' "$0" | sed 's/^# \{0,1\}//'
}

qqsp_require_tool clang-tidy
qqsp_require_tool python3 "python3 is used to read compile_commands.json"
[ -f "$QQSP_BUILD_DIR/compile_commands.json" ] || qqsp_die_usage "no compile database; run: cmake --preset $QQSP_PRESET_DEFAULT"

SINCE=""
FIX=0
NO_FAIL=0
JOBS="${QQSP_JOBS:-4}"
CHECKS=""
ENGINE=0
PATHS=()

while [ $# -gt 0 ]; do
  case "$1" in
    --since) SINCE="${2:?--since needs a ref}"; shift 2 ;;
    --fix) FIX=1; shift ;;
    --no-fail) NO_FAIL=1; shift ;;
    --jobs) JOBS="${2:?--jobs needs a number}"; shift 2 ;;
    --with-engine) ENGINE=1; shift ;;
    --checks) CHECKS="${2:?--checks needs a spec}"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    --) shift; PATHS=("$@"); break ;;
    *) qqsp_die_usage "unknown argument: $1" ;;
  esac
done

cd "$QQSP_REPO_ROOT"
FILES_PY="$SELF_DIR/lib/files.py"

# --- select files ------------------------------------------------------------

if [ ${#PATHS[@]} -gt 0 ]; then
  FILES=("${PATHS[@]}")
else
  mapfile -d '' FILES < <("$FILES_PY" --app-cxx --db "$QQSP_BUILD_DIR/compile_commands.json")
  if [ "$ENGINE" -eq 1 ]; then
    mapfile -d '' ENGINE_FILES < <("$FILES_PY" --engine-c --db "$QQSP_BUILD_DIR/compile_commands.json")
    FILES+=("${ENGINE_FILES[@]}")
  fi
fi

if [ -n "$SINCE" ]; then
  [ ${#PATHS[@]} -eq 0 ] || qqsp_die_usage "--since cannot be combined with explicit paths"
  CHANGED="$QQSP_LOG_DIR/tidy-changed.nul"
  mkdir -p "$QQSP_LOG_DIR"
  git diff --name-only -z --diff-filter=ACMR "$SINCE" -- src/ >"$CHANGED" || true
  FILTERED="$(mktemp "$QQSP_LOG_DIR/tidy-filtered.XXXXXX")"
  printf '%s\0' "${FILES[@]}" | sort -z >"$FILTERED"
  mapfile -d '' FILES < <(comm -12 -z <(sort -z <"$CHANGED") <(sort -z "$FILTERED") || true)
  rm -f "$FILTERED"
fi

COUNT=${#FILES[@]}
[ "$COUNT" -gt 0 ] || { qqsp_ok "no files to analyze"; exit 0; }
echo "Analyzing $COUNT file(s)..." >&2

LOG="$QQSP_LOG_DIR/tidy.log"
mkdir -p "$QQSP_LOG_DIR"
: >"$LOG"

run_tidy() {
  # Engine C files get a minimal C-oriented set; C++ files use .clang-tidy.
  if [ "$1" = "engine" ]; then
    clang-tidy -p "$QQSP_BUILD_DIR" --quiet \
      --config="{Checks: 'bugprone-*,cert-*,misc-*,readability-redundant-*,readability-simplify-*, HeaderFilterRegex: \"/src/qsp/\", WarningsAsErrors: \"\", FormatStyle: \"none\"}" \
      "$2"
  else
    local extra=()
    [ -n "$CHECKS" ] && extra=(--checks "$CHECKS")
    [ "$FIX" -eq 1 ] && extra+=(--fix)
    clang-tidy -p "$QQSP_BUILD_DIR" --config-file "$REPO_TIDY_CONFIG" --quiet "${extra[@]}" "$2"
  fi
}
REPO_TIDY_CONFIG="$QQSP_REPO_ROOT/.clang-tidy"

set +e
for f in "${FILES[@]}"; do
  case "$ENGINE" in
    1) case "$f" in
         "$QQSP_ENGINE_DIR"/*) run_tidy engine "$f" >>"$LOG" 2>&1 ;;
         *) run_tidy cxx "$f" >>"$LOG" 2>&1 ;;
       esac ;;
    *) run_tidy cxx "$f" >>"$LOG" 2>&1 ;;
  esac
done
set -e
# NOTE: files are analyzed sequentially for deterministic merged output; use
# --jobs via parallel shells only if log interleaving is acceptable.

WARNINGS_LOG="$QQSP_LOG_DIR/tidy-warnings.txt"
grep -E ':[0-9]+:[0-9]+: warning: ' "$LOG" | qqsp_relpath | sort -u >"$WARNINGS_LOG" || true
FINDINGS="$(wc -l <"$WARNINGS_LOG")"

qqsp_section "summary"
echo "findings: $FINDINGS unique warning(s) across $COUNT file(s)"
if [ "$FINDINGS" -gt 0 ]; then
  qqsp_section "per-check counts"
  grep -oE '\[[a-zA-Z0-9-]+\]$' "$WARNINGS_LOG" | sort | uniq -c | sort -rn || true
  qqsp_section "findings"
  if [ "$FINDINGS" -le 300 ]; then
    cat "$WARNINGS_LOG"
  else
    head -n 300 "$WARNINGS_LOG"
    echo "...($((FINDINGS - 300)) more; full list: $WARNINGS_LOG)"
  fi
fi

if [ "$FIX" -eq 1 ]; then
  echo
  echo "Fixes applied where possible. Next steps:"
  echo "  git diff          # review every fix before keeping it"
  echo "  tools/scripts/check.sh   # confirm the build still passes"
fi

if [ "$FINDINGS" -gt 0 ] && [ "$NO_FAIL" -eq 0 ] && [ "$FIX" -eq 0 ]; then
  exit 1
fi
qqsp_ok "tidy done ($FINDINGS finding(s))"
