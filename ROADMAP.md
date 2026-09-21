# Qqsp Modernization Roadmap

## Conventions

* Milestones run in list order, which matches their `depends_on` order. Work on a
  milestone only when its dependencies are `done`, unless the task explicitly targets it.
* `status` is `pending`, `in_progress`, or `done`. Set `done` only when the gate is met
  and the result of the checks defined in `AGENTS.md` is cited in the change that closes
  the milestone. Update `status` in the change that starts or closes it.

---

## M1 — Establish baseline

status: pending

depends_on: none

objective:
Record the current build and deterministic verification state before modernization changes.

tasks:

* Build the project.
* Run existing deterministic tests and checks.
* Record the results, including relevant pre-existing failures, in the Baseline section below.

gate:

* Baseline results are recorded.
* Pre-existing failures are distinguishable from regressions.

---

## M2 — Pre-migration cleanup

status: pending

depends_on: M1

objective:
Remove code and configuration that is demonstrably obsolete before migration work.

tasks:

* Remove unused source files, symbols, code paths, definitions, and configuration.
* Remove obsolete dependencies.
* Remove remaining references to the bundled Oniguruma sources and submodule.
* Remove the legacy Qt WebKit backend, which is not supported: its sources, build
  option, compile definitions, and Qt components that no other code needs, together with
  conditionals that only existed to share code between the two web backends.
* Remove other abandoned features and obsolete platform-specific code.
* Remove workarounds whose removal does not depend on later migrations.
* Remove stale documentation associated with removed functionality.

gate:

* Removed items are supported by evidence.
* Build succeeds.
* Deterministic checks pass.

---

## M3 — Validate CMake/Ninja

status: pending

depends_on: M2

objective:
Establish CMake/Ninja as the maintained build path and verify that it represents the current project requirements.

tasks:

* Verify source composition and generated files, including translation generation and
  resource embedding.
* Verify compile definitions and language standards, including that each definition is
  valid for the Qt and compiler versions in use.
* Verify that compiler-warning suppressions are limited to the code that requires them.
* Verify Qt dependencies and target relationships.
* Verify platform conditions and optional configurations.
* Verify bundled QSP source inclusion.
* Verify that each maintained configuration has an explicit build type in
  `CMakePresets.json`.
* Keep installation, deployment, and packaging out of CMake (no `install()` rules or
  deployment logic); external packaging owns them. Verify that the build produces what
  packaging consumes, and that the packaging inputs kept in the repository (desktop
  entry, MIME definition, icons) are mutually consistent.
* Maintain compilation database generation.
* Validate relevant configurations.

gate:

* Required CMake/Ninja configurations build successfully.
* Deterministic checks pass.

---

## M4 — Remove qmake

status: pending
note: qmake project files are already absent from the tree; verify that no qmake logic or
references remain.

depends_on: M3

objective:
Remove the remaining obsolete qmake build path.

tasks:

* Remove qmake project and configuration files.
* Remove qmake-specific build logic and references.

gate:

* CMake/Ninja is the only maintained build path.
* Deterministic checks pass.

---

## M5 — Add initial regression coverage

status: pending

depends_on: M4

objective:
Add automated coverage for important deterministic behavior before the language and Qt
migrations, so regressions introduced by later milestones are caught as they occur.

tasks:

* Add CTest-based test infrastructure to the CMake build.
* Include the QSP regular-expression functions, whose behavior comes from the system
  Oniguruma.
* Identify important behavior lacking practical coverage, prioritizing externally
  observable behavior in the areas listed under M8 `focus_areas`.
* Add focused regression tests or checks that exercise behavior through interfaces that
  survive the migrations, so the same tests validate both sides.
* Add the tests to the deterministic checks defined in `AGENTS.md`.
* Record important cases that remain impractical to automate.

gate:

* Important practical compatibility risks have regression coverage.
* Deterministic checks include the tests and pass.

---

## M6 — Migrate C++14 to C++17

status: pending

depends_on: M5

objective:
Migrate application C++ code to C++17 while remaining on Qt 5.

tasks:

* Change the application language standard to C++17 everywhere it is named: build
  configuration, presets, `.clang-format`, and the verification commands in `AGENTS.md`.
* Resolve migration-related compilation and compatibility issues.
* Apply required C++17 changes.
* Preserve the C11 QSP engine boundary.

gate:

* C++17 build succeeds.
* Deterministic checks pass.

---

## M7 — Prepare for Qt 6

status: pending

depends_on: M6

objective:
Remove known in-scope Qt 6 migration blockers while remaining on Qt 5.

tasks:

* Identify Qt 6 migration blockers.
* Enforce removal of APIs deprecated through Qt 5.15 at compile time
  (`QT_DISABLE_DEPRECATED_BEFORE` on Qt 5.15) and resolve the resulting errors.
* Replace or adapt blocking APIs and constructs.
* Verify WebEngine page/profile ownership and lifetime.
* Verify affected code paths.

gate:

* No known in-scope Qt 6 migration blocker remains.
* Deterministic checks pass.

---

## M8 — Migrate Qt 5 to Qt 6

status: pending

depends_on: M7

objective:
Migrate the application to Qt 6.

tasks:

* Update Qt dependencies, CMake configuration, presets, and the verification commands in
  `AGENTS.md`.
* Move deprecation enforcement to `QT_DISABLE_DEPRECATED_UP_TO` (Qt 6.5 and later).
* Replace removed or changed Qt APIs.
* Update generated-code integration where required.
* Update affected platform-specific code.
* Verify affected Qt-dependent subsystems.

focus_areas:

* WebEngine/WebChannel
* Multimedia
* Widgets
* Resources
* Signals/slots
* QObject ownership/lifetime
* Platform integration

gate:

* Qt 6 build succeeds.
* Deterministic checks pass.

---

## M9 — Post-migration cleanup

status: pending

depends_on: M8

objective:
Remove artifacts made obsolete by the completed migrations.

tasks:

* Reassess dead and unused code.
* Remove obsolete Qt 5 compatibility code.
* Remove migration-only shims and workarounds.
* Remove transitional build logic.
* Remove obsolete platform and configuration branches.
* Remove unnecessary dependencies.
* Remove superseded implementations.
* Remove stale documentation associated with removed functionality.

gate:

* Removed items are supported by evidence.
* Build succeeds.
* Deterministic checks pass.

---

## M10 — Strengthen regression coverage

status: pending

depends_on: M9

objective:
Extend automated coverage to behavior affected by the migrations and cleanup that the
initial coverage did not reach.

tasks:

* Reassess coverage gaps after the migrations and cleanup.
* Add focused regression tests or checks for the remaining important behavior, including
  the M8 `focus_areas`.
* Update the record of cases that remain impractical to automate.

gate:

* Important practical compatibility risks have regression coverage.
* Deterministic checks pass.

---

## M11 — Refine dependencies and CMake

status: pending

depends_on: M9

objective:
Simplify the final dependency and build structure.

tasks:

* Review direct and transitive dependencies.
* Remove unnecessary dependencies after verification.
* Improve target relationships.
* Simplify obsolete CMake configuration.
* Align build dependencies with actual requirements.

gate:

* Clean configuration succeeds.
* Full build succeeds.
* Deterministic checks pass.

---

## M12 — Final audit

status: pending

depends_on: M10, M11

objective:
Verify completion of the modernization roadmap.

audit_targets:

* qmake removal
* CMake/Ninja
* C++17
* Qt 6
* pre-migration cleanup
* post-migration cleanup
* dependency structure
* regression coverage

tasks:

* Verify milestone completion evidence.
* Identify incomplete work and known limitations.
* Verify no obsolete migration artifacts remain.

gate:

* All required milestones have implementation and verification evidence.
* No unresolved critical technical regression remains.

---

## Baseline

Recorded by M1. Later work compares against it to classify failures as pre-existing or
regressions.

* commit:
* toolchain (CMake, compiler, Qt, Oniguruma):
* build result:
* warnings:
* pre-existing failures:
