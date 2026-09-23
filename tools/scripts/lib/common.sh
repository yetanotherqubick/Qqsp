# Shared helpers for tools/scripts. This file is sourced, not executed.
#
# Conventions used across the suite:
#   exit 0  success (clean run, no findings)
#   exit 1  findings or failure (build error, lint findings, new warnings)
#   exit 2  missing tool or usage error
# Logs and scratch files live under build/ (gitignored).

QQSP_SCRIPTS_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
QQSP_REPO_ROOT="$(cd "$QQSP_SCRIPTS_DIR/../.." && pwd)"
QQSP_BUILD_DIR="$QQSP_REPO_ROOT/build"
QQSP_LOG_DIR="$QQSP_BUILD_DIR/logs"
QQSP_PRESET_DEFAULT="qt5-cpp14"

# Application source areas. The bundled engine tree (src/qsp) is excluded by
# default everywhere: per AGENTS.md it stays unchanged unless a task targets it.
QQSP_APP_AREAS=(src/app src/web src/widgets src/dialogs src/multimedia src/platform)
QQSP_ENGINE_DIR="src/qsp"

# --- basic reporting ---------------------------------------------------------

qqsp_ok() { echo "OK: $*"; }
qqsp_fail() { echo "FAIL: $*" >&2; }
qqsp_section() { echo; echo "### $*"; }

qqsp_die_usage() {
  echo "ERROR: $*" >&2
  echo "Run with --help for usage." >&2
  exit 2
}

# Die if a tool is missing. $1 tool name, $2 optional install hint.
qqsp_require_tool() {
  local name="$1" hint="${2:-}"
  if ! command -v "$name" >/dev/null 2>&1; then
    echo "ERROR: required tool '$name' not found in PATH.${hint:+ Install hint: $hint}" >&2
    exit 2
  fi
}

# First line of a command's output, empty on failure (for env inventories).
qqsp_first_line() {
  "$@" 2>/dev/null | head -n 1 || true
}

# Make repo-relative paths in stdin text.
qqsp_relpath() {
  sed "s|$QQSP_REPO_ROOT/||g"
}

# --- shared flag parsing -----------------------------------------------------

qqsp_help_requested() {
  case "${1:-}" in
    -h|--help) return 0 ;;
    *) return 1 ;;
  esac
}

# --- file selection ----------------------------------------------------------

# Tracked source files under the given pathspecs as NUL-separated names
# (repo-relative). Generated files (moc_*, qrc_*, ui_*) are always excluded.
qqsp_git_sources() {
  git -C "$QQSP_REPO_ROOT" ls-files -z -- "$@" |
    while IFS= read -r -d '' f; do
      case "$f" in
        */moc_*|*/qrc_*|*/ui_*|moc_*|qrc_*|ui_*) continue ;;
        *.c|*.cpp|*.cc|*.cxx|*.h|*.hh|*.hpp) printf '%s\0' "$f" ;;
      esac
    done
}

# Tracked application sources (engine excluded). Set QQSP_WITH_ENGINE=1 in the
# environment to append the engine tree.
qqsp_app_sources() {
  qqsp_git_sources "${QQSP_APP_AREAS[@]}"
  if [ "${QQSP_WITH_ENGINE:-0}" = "1" ]; then
    qqsp_git_sources "$QQSP_ENGINE_DIR"
  fi
}

# --- warnings and errors -----------------------------------------------------

# Extract warning lines ("path:line:col: warning: ...") from a build/lint log
# on stdin, normalized to repo-relative paths. Always succeeds (a log with no
# warnings must not trip pipefail in the caller's pipelines).
qqsp_warning_lines() {
  { grep -E ':[0-9]+:[0-9]+: warning: ' || true; } | qqsp_relpath
}

# Extract error lines ("path:line:col: error: ...") from a log on stdin,
# normalized to repo-relative paths. Always succeeds.
qqsp_error_lines() {
  { grep -E ':[0-9]+:[0-9]+: (fatal )?error: ' || true; } | qqsp_relpath
}
