# AGENTS.md

## Scope

Work only within the requested task. Do not introduce unrelated refactoring, cleanup,
dependency changes, architectural changes, or behavior changes.

Use `ROADMAP.md` to determine modernization scope and milestone dependencies.

## Repository boundaries

Preserve the C11 QSP engine boundary unless the task explicitly targets the engine.

When modifying the Qt application, do not make unrelated changes under `qsp/`.

Do not edit generated artifacts when they should be regenerated from their source configuration.

## Evidence

Prefer project-local evidence in this order:

1. repository source and configuration
2. build metadata and compilation database
3. tests, checks, compiler output, and generated build information
4. Graft analysis
5. installed development files
6. external documentation when local evidence is insufficient or explicitly required

Treat analysis and tool output as evidence, not proof. Verify conclusions that affect
changes against the applicable source, configuration, build, or test evidence.

Distinguish pre-existing failures from regressions introduced by the current change.

Do not claim verification that was not performed.

## Scope discipline

Leave newly discovered work for a separate task unless it is required to complete the
current task.

<!-- graft:start -->
## Graft — repo context graph

This repo is indexed in `graft/`: small linked markdown nodes that explain each
system and carry exact file:line spans, kept in sync with the code through git.

For ANY task here — understanding how something works, finding where code lives,
or scoping a change — get context from the graph before grepping or opening
source files. Re-ask freely (it's cheap) and reuse literal identifiers you
already have (symbol, error string, file name) as the query. New to this repo?
Run `graft map` first — a token-budgeted orientation (dir clusters, hubs,
hotspots), no LLM, no key.

- Run `graft ask "<your question>" --source` → ranked nodes with the relevant
  code spans inlined (each hit's ≤8-line crux by default; `--full` for whole
  definitions when the crux isn't enough). Match the tool to the task shape:
  for understanding or editing, the top node IS the answer — cite its
  `covers:` file:line spans and edit straight from `--source`. For
  exhaustive tasks ("every occurrence / every caller of this pattern"), ranked
  results are top-N, not complete — run `graft grep "<literal>"` instead
  (exhaustive over indexed files, grouped by enclosing symbol), falling back
  to raw `grep -rn` only for unindexed files.
- `graft skeleton <file>` → every definition's signature + span, ~10× cheaper
  than reading the file; use it to skim an API surface.
- `graft callers <symbol>` gives precomputed, exact edges — who calls this.
  Add `--direction out` for what it calls, or `--depth N` to walk
  transitively for the full blast radius. For structural questions, skip
  ranking and use this directly.
- Or browse: `graft/INDEX.md` lists every node; follow the links.
- Monorepos and folders of multiple repos rank fairly across sub-projects —
  hits carry `[scope/]` labels naming which one they're from. Narrow with
  `graft ask "<task>" --in <scope>/` once you know where you're working.

If a returned span is truncated ("+N more lines"), open the file at that exact
range before finalizing. Only open source files when a node genuinely lacks a
needed detail, and then at the exact file:line the node points to — never
re-read whole files.

After big code changes, refresh the graph with `graft build` (deterministic,
no API key, $0).
<!-- graft:end -->
