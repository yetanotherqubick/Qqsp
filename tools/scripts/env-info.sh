#!/usr/bin/env bash
# Print a one-shot toolchain inventory for this machine.
#
# Usage: env-info.sh [--markdown]
#   --markdown  emit a fenced block suitable for pasting into the ROADMAP.md
#               Baseline section (M1).
#
# Missing tools are reported as "-" (this is an inventory, not a gate).
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

MARKDOWN=0
if [ "${1:-}" = "--markdown" ]; then
  MARKDOWN=1
elif [ $# -gt 0 ]; then
  qqsp_die_usage "unknown argument: $1"
fi

qqsp_require_tool cmake "install CMake >= 3.25"

# ver <label> <cmd...>  -> "<label>: <first output line>" or "-"
ver() {
  local label="$1"
  shift
  local out
  out="$(qqsp_first_line "$@")"
  if [ "$MARKDOWN" -eq 1 ]; then
    printf '%s: %s\n' "$label" "${out:--}"
  else
    printf '%-24s %s\n' "$label" "${out:--}"
  fi
}

# ver_compact <label> <cmd...> -> multi-line banners (clang-tidy) on one line.
ver_compact() {
  local label="$1"
  shift
  local out
  out="$("$@" 2>/dev/null | sed '/^$/d' | tr '\n' ' ' | sed 's/  */ /g; s/ $//')" || true
  if [ "$MARKDOWN" -eq 1 ]; then
    printf '%s: %s\n' "$label" "${out:--}"
  else
    printf '%-24s %s\n' "$label" "${out:--}"
  fi
}

inventory() {
  ver "cmake" cmake --version
  ver "ninja" ninja --version
  ver "cc (default C)" cc --version
  ver "c++ (default C++)" c++ --version
  ver "clang" clang --version
  ver "clang++" clang++ --version
  ver_compact "clang-format" clang-format --version
  ver_compact "clang-tidy" clang-tidy --version
  ver_compact "clazy" clazy --version
  ver_compact "clazy-standalone" clazy-standalone --version
  ver "cppcheck" cppcheck --version
  ver "Qt (qmake)" qmake -query QT_VERSION
  ver "Qt (qmake6)" qmake6 -query QT_VERSION
  ver "oniguruma (pkg-config)" pkg-config --modversion oniguruma
  ver "git" git --version
}

if [ "$MARKDOWN" -eq 1 ]; then
  echo '```'
  inventory
  echo '```'
else
  inventory
fi
