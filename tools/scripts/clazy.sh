#!/usr/bin/env bash
# Run clazy-standalone (Qt-specific static analysis) over application C++
# sources using the build's compile database.
#
# Usage: clazy.sh [--level 0|1|2] [--checks SPEC] [--no-fail] [--since REF]
#                 [-- paths...] [--help]
#
#   --level N    clazy check level (default 1); level 2 is verbose, opt-in
#   --checks     explicit clazy check list (overrides --level)
#
# Exit codes: 0 clean (or --no-fail), 1 findings, 2 usage/missing tool.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,17p' "$0" | sed 's/^# \{0,1\}//'
}

qqsp_require_tool clazy-standalone "install the clazy package (Qt lint framework)"
qqsp_require_tool python3 "python3 is used to read compile_commands.json"
[ -f "$QQSP_BUILD_DIR/compile_commands.json" ] || qqsp_die_usage "no compile database; run: cmake --preset $QQSP_PRESET_DEFAULT"

LEVEL="1"
CHECKS=""
NO_FAIL=0
SINCE=""
PATHS=()

while [ $# -gt 0 ]; do
  case "$1" in
    --level) LEVEL="${2:?--level needs 0|1|2}"; shift 2 ;;
    --checks) CHECKS="${2:?--checks needs a spec}"; shift 2 ;;
    --no-fail) NO_FAIL=1; shift ;;
    --since) SINCE="${2:?--since needs a ref}"; shift 2 ;;
    -h|--help) usage; exit 0 ;;
    --) shift; PATHS=("$@"); break ;;
    *) qqsp_die_usage "unknown argument: $1" ;;
  esac
done

cd "$QQSP_REPO_ROOT"

if [ ${#PATHS[@]} -gt 0 ]; then
  FILES=("${PATHS[@]}")
else
  mapfile -d '' FILES < <("$SELF_DIR/lib/files.py" --app-cxx --db "$QQSP_BUILD_DIR/compile_commands.json")
fi

if [ -n "$SINCE" ]; then
  [ ${#PATHS[@]} -eq 0 ] || qqsp_die_usage "--since cannot be combined with explicit paths"
  CHANGED="$QQSP_LOG_DIR/clazy-changed.nul"
  mkdir -p "$QQSP_LOG_DIR"
  git diff --name-only -z --diff-filter=ACMR "$SINCE" -- "${QQSP_APP_AREAS[@]}" >"$CHANGED" || true
  FILTERED="$(mktemp "$QQSP_LOG_DIR/clazy-filtered.XXXXXX")"
  printf '%s\0' "${FILES[@]}" | sort -z >"$FILTERED"
  mapfile -d '' FILES < <(comm -12 -z <(sort -z <"$CHANGED") <(sort -z "$FILTERED") || true)
  rm -f "$FILTERED"
fi

COUNT=${#FILES[@]}
[ "$COUNT" -gt 0 ] || { qqsp_ok "no files to analyze"; exit 0; }
echo "Analyzing $COUNT file(s) at level $LEVEL..." >&2

LOG="$QQSP_LOG_DIR/clazy.log"
mkdir -p "$QQSP_LOG_DIR"
: >"$LOG"

CLAZY_ARGS=(-p "$QQSP_BUILD_DIR" -header-filter='^src/')
if [ -n "$CHECKS" ]; then
  CLAZY_ARGS+=(-checks="$CHECKS")
else
  CLAZY_ARGS+=(-checks="level$LEVEL")
fi

set +e
for f in "${FILES[@]}"; do
  clazy-standalone "${CLAZY_ARGS[@]}" "$f" >>"$LOG" 2>&1
done
set -e

WARNINGS_LOG="$QQSP_LOG_DIR/clazy-warnings.txt"
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

if [ "$FINDINGS" -gt 0 ] && [ "$NO_FAIL" -eq 0 ]; then
  exit 1
fi
qqsp_ok "clazy done ($FINDINGS finding(s))"
