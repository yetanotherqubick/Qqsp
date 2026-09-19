# Qqsp Modernization Roadmap

## M1 — Establish baseline

depends_on: none

objective:
Record the current build and deterministic verification state before modernization changes.

tasks:

* Build the project.
* Run existing deterministic tests and checks.
* Record relevant pre-existing failures.

gate:

* Baseline results are recorded.
* Pre-existing failures are distinguishable from regressions.

---

## M2 — Pre-migration cleanup

depends_on: M1

objective:
Remove code and configuration that is demonstrably obsolete before migration work.

tasks:

* Remove unused source files, symbols, code paths, definitions, and configuration.
* Remove obsolete dependencies.
* Remove abandoned features and obsolete platform-specific code.
* Remove workarounds whose removal does not depend on later migrations.
* Remove stale documentation associated with removed functionality.

gate:

* Removed items are supported by evidence.
* Build succeeds.
* Deterministic checks pass.

---

## M3 — Validate CMake/Ninja

depends_on: M2

objective:
Establish CMake/Ninja as the maintained build path and verify that it represents the current project requirements.

tasks:

* Verify source composition and generated files.
* Verify compile definitions and language standards.
* Verify Qt dependencies and target relationships.
* Verify platform conditions and optional configurations.
* Verify bundled QSP source inclusion.
* Maintain compilation database generation.
* Validate relevant configurations.

gate:

* Required CMake/Ninja configurations build successfully.
* Deterministic checks pass.

---

## M4 — Remove qmake

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

## M5 — Migrate C++14 to C++17

depends_on: M4

objective:
Migrate application C++ code to C++17 while remaining on Qt 5.

tasks:

* Change the application language standard to C++17.
* Resolve migration-related compilation and compatibility issues.
* Apply required C++17 changes.
* Preserve the C11 QSP engine boundary.

gate:

* C++17 build succeeds.
* Deterministic checks pass.

---

## M6 — Prepare for Qt 6

depends_on: M5

objective:
Remove known in-scope Qt 6 migration blockers while remaining on Qt 5.

tasks:

* Identify Qt 6 migration blockers.
* Replace or adapt blocking APIs and constructs.
* Verify affected code paths.

gate:

* No known in-scope Qt 6 migration blocker remains.
* Deterministic checks pass.

---

## M7 — Migrate Qt 5 to Qt 6

depends_on: M6

objective:
Migrate the application to Qt 6.

tasks:

* Update Qt dependencies and CMake configuration.
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

## M8 — Post-migration cleanup

depends_on: M7

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

## M9 — Add compatibility regression coverage

depends_on: M4
strengthen_after: M8

objective:
Add automated coverage for important deterministic behavior affected by modernization.

tasks:

* Identify important behavior lacking practical coverage.
* Add focused regression tests or checks.
* Prioritize externally observable behavior.
* Record important cases that remain impractical to automate.

gate:

* Important practical compatibility risks have regression coverage.

---

## M10 — Refine dependencies and CMake

depends_on: M8

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

## M11 — Final audit

depends_on: M9, M10

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
