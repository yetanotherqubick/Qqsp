# tools/scripts — developer and agent tooling

One command per repetitive job in the modernization workflow. Every script:
`--help`, quiet on success, deduped findings,
stable `### section` headers, and exit codes agents can branch on.

## Conventions

* **Exit codes**: `0` clean · `1` findings/failure · `2` missing tool or usage
  error.
* **Scope**: application areas only (`src/app`, `src/web`, `src/widgets`,
  `src/dialogs`, `src/multimedia`, `src/platform`). Generated code
  (`moc_*`, `qrc_*`, `ui_*`) is always excluded.
* **Files**: scripts never modify sources, except `format.sh --fix` and
  `tidy.sh --fix` when explicitly requested.
* **Scratch files** live under `build/` (gitignored): logs in `build/logs/`,
  warning baselines in `build/baseline/`, ratchet builds in
  `build/qt-deprecated/`.
* Lint findings (`tidy.sh`, `clazy.sh`) are a **worklist, not a gate**: the
  deterministic check per `AGENTS.md` is `check.sh` (configure + build, later
  + CTest).

## Scripts

| Script | Purpose | Example |
|---|---|---|
| `env-info.sh` | Toolchain inventory | `env-info.sh --markdown` |
| `check.sh` | The gate: configure + build (+ ctest when it exists), warning capture | `check.sh --record-baseline` then `check.sh --compare-baseline` |
| `format.sh` | clang-format check/fix | `format.sh --fix -- src/app/mainwindow.cpp` |
| `tidy.sh` | clang-tidy over the compile database | `tidy.sh --since HEAD~5` |
| `clazy.sh` | clazy-standalone Qt analysis | `clazy.sh --level 1` |
| `qt-deprecated.sh` | Deprecation ratchet build | `qt-deprecated.sh --up-to 0x060000` → must reach 0 |
| `todo.sh` | TODO/FIXME/XXX/HACK inventory | `todo.sh --all` |
| `refs.sh` | Removal evidence for a symbol | `refs.sh QspInputDlg` |

## Typical workflows

**Baseline** — record what "normal" looks like so later failures are
classifiable as pre-existing or regressions:

```sh
tools/scripts/env-info.sh --markdown   # toolchain inventory
tools/scripts/check.sh --record-baseline
tools/scripts/check.sh --compare-baseline   # every later task: prints only NEW warnings
```

**Before removing code** — gather evidence first, then clean up:

```sh
tools/scripts/refs.sh <symbol> --graft
tools/scripts/tidy.sh --fix --since main   # review fixes, then check.sh
```

**Qt 6 readiness**:

```sh
tools/scripts/clazy.sh --level 1
tools/scripts/qt-deprecated.sh --up-to 0x060000
```

The ratchet value is the CMake cache variable
`QQSP_QT_DISABLE_DEPRECATED_UP_TO` (default `0x050F00` = the currently
enforced Qt 5.15 level); the script raises it in a scratch build only.

## Configuration

* `.clang-format` (repository root) — formatting; `format.sh` follows it.
* `.clang-tidy` (repository root) — curated check list; exclusions need a
  reason.
* `lib/common.sh` — shared scope lists and helpers; `lib/files.py` — source
  lists derived from `build/compile_commands.json` (the lint scripts only see
  what the current build configuration actually compiles).

## Requirements

`cmake`, `ninja`, `git`, `pkg-config`, a C compiler, and the toolchain the
build needs (Qt 5.15, Oniguruma). Optional but used when present:
`clang-format`, `clang-tidy`, `clazy-standalone`, `python3` (for
`tidy.sh`/`clazy.sh` file selection), `graft` (`refs.sh --graft`). Missing
tools produce a one-line install hint (exit 2).
