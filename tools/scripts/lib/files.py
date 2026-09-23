#!/usr/bin/env python3
"""Emit source-file lists derived from the build's compile_commands.json.

Used by the lint scripts (tidy.sh, clazy.sh) so that file selection follows the
actual build configuration (options, enabled backends, generated code) instead
of globbing the tree.

Output: one NUL-separated repo-relative path per selected entry.
Exit codes: 0 ok, 2 missing/stale compile database.

Usage:
  files.py --app-cxx           C++ sources under application areas
  files.py --engine-c          C sources under src/qsp
  files.py --other             everything else in the database (debug aid)
"""

import argparse
import json
import os
import sys

APP_AREAS = (
    "src/app",
    "src/web",
    "src/widgets",
    "src/dialogs",
    "src/multimedia",
    "src/platform",
)
ENGINE_DIR = "src/qsp"
GENERATED_PREFIXES = ("moc_", "qrc_", "ui_")

CXX_EXTS = (".cpp", ".cc", ".cxx")
C_EXTS = (".c",)


def load_entries(db_path):
    """Return repo-relative source paths from the compile database."""
    db_path = os.path.abspath(db_path)
    repo_root = os.path.dirname(os.path.dirname(db_path))  # build/ -> repo root
    try:
        with open(db_path, encoding="utf-8") as f:
            entries = json.load(f)
    except FileNotFoundError:
        sys.exit(f"ERROR: {db_path} not found. Run: cmake --preset qt5-cpp14")
    except json.JSONDecodeError as exc:
        sys.exit(f"ERROR: {db_path} is not valid JSON ({exc}). Re-run configure.")

    files = []
    for entry in entries:
        path = os.path.normpath(
            os.path.join(entry.get("directory", repo_root), entry["file"])
        )
        if path.startswith(repo_root + os.sep):
            path = os.path.relpath(path, repo_root)
        files.append(path.replace(os.sep, "/"))
    return files, repo_root


def is_generated(path):
    return os.path.basename(path).startswith(GENERATED_PREFIXES)


def main():
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--db", default="build/compile_commands.json",
                        help="path to compile_commands.json (default: %(default)s)")
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("--app-cxx", action="store_true",
                       help="C++ sources under application areas")
    group.add_argument("--engine-c", action="store_true",
                       help="C sources under the engine tree")
    group.add_argument("--other", action="store_true",
                       help="database entries outside both categories")
    args = parser.parse_args()

    files, _ = load_entries(args.db)

    selected = []
    for path in files:
        if is_generated(path):
            continue
        if args.app_cxx:
            if path.startswith(APP_AREAS) and path.endswith(CXX_EXTS):
                selected.append(path)
        elif args.engine_c:
            if path.startswith(ENGINE_DIR + "/") and path.endswith(C_EXTS):
                selected.append(path)
        else:  # --other
            in_app = path.startswith(APP_AREAS) and path.endswith(CXX_EXTS)
            in_engine = path.startswith(ENGINE_DIR + "/") and path.endswith(C_EXTS)
            if not (in_app or in_engine):
                selected.append(path)

    for path in sorted(set(selected)):
        sys.stdout.write(path + "\0")


if __name__ == "__main__":
    main()
