#!/usr/bin/env bash
# Removal-evidence helper: locate every tracked reference to an identifier and
# classify them, so a cleanup change can cite concrete evidence (ROADMAP M2/M9
# gates: "Removed items are supported by evidence").
#
# Usage: refs.sh <identifier> [--graft] [--help]
#   --graft  also ask the graft graph for caller edges (best-effort)
#
# Output: code references, documentation references, and a verdict line.
# Word-boundary matching; build/, graft/, and untracked files are excluded.
# Exit codes: 0 ok (references are data, not failures), 2 usage error.
set -euo pipefail

SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=lib/common.sh
source "$SELF_DIR/lib/common.sh"

usage() {
  sed -n '2,11p' "$0" | sed 's/^# \{0,1\}//'
}

ID="${1:-}"
[ -n "$ID" ] || qqsp_die_usage "missing identifier"
shift || true

GRAFT=0
while [ $# -gt 0 ]; do
  case "$1" in
    --graft) GRAFT=1; shift ;;
    -h|--help) usage; exit 0 ;;
    *) qqsp_die_usage "unknown argument: $1" ;;
  esac
done

cd "$QQSP_REPO_ROOT"

RESULTS="$(git grep -nw "$ID" || true)"

if [ -z "$RESULTS" ]; then
  qqsp_ok "'$ID' has no tracked references"
  exit 0
fi

CODE="$(echo "$RESULTS" | grep -vE '^[^:]+\.(md|txt|json):' || true)"
DOCS="$(echo "$RESULTS" | grep -E '^[^:]+\.(md|txt|json):' || true)"
CODE_FILES="$(echo "$CODE" | cut -d: -f1 | sort -u | grep -c . || true)"
DOC_FILES="$(echo "$DOCS" | cut -d: -f1 | sort -u | grep -c . || true)"

qqsp_section "code references ($CODE_FILES file(s))"
if [ -n "$CODE" ]; then
  echo "$CODE"
else
  echo "(none)"
fi

if [ -n "$DOCS" ]; then
  qqsp_section "documentation references ($DOC_FILES file(s))"
  echo "$DOCS"
fi

qqsp_section "verdict"
case "$CODE_FILES" in
  0) echo "'$ID': referenced only in documentation - candidate for coordinated doc+code removal" ;;
  1) echo "'$ID': referenced in a single code file - review that file for self-references only" ;;
  *) echo "'$ID': referenced across $CODE_FILES code file(s) - removal needs caller analysis" ;;
esac

if [ "$GRAFT" -eq 1 ]; then
  qqsp_section "graft callers"
  if command -v graft >/dev/null 2>&1; then
    graft callers "$ID" || echo "(graft returned no data; falls back to the references above)"
  else
    echo "(graft not available in PATH)"
  fi
fi
