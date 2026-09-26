cmake_minimum_required(VERSION 3.25)

project(Qqsp
    VERSION 1.9.0.0
    LANGUAGES CXX
)

# Export the compilation database for clangd and other analysis tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the language standard used by the application.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt code generation.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(QQSP_ENABLE_WEBBOX
    "Build the Qt WebEngine-based web box"
    ON
)

# Compile-time deprecation enforcement for the application target. On Qt 6
# the effective macro is QT_DISABLE_DEPRECATED_UP_TO; the deprecation ratchet
# (tools/scripts/qt-deprecated.sh, ROADMAP M7/M8) raises the value in scratch
# builds.
set(QQSP_QT_DISABLE_DEPRECATED 0x060500
    CACHE STRING "Qt API version up to which deprecated APIs are disabled"
)

# The QSP engine comes from the system qsp-legacy library, consumed the same
# way the build previously consumed the system Oniguruma. The package exports
# the target QspLegacy::qsp-legacy (a shared library linking Oniguruma
# privately) together with the public headers qsp.h, qsp_default.h, and
# bindings_config.h.
#
# cmake/Findoniguruma.cmake stays and the module path is extended first: the
# installed QspLegacyConfig.cmake calls find_dependency(oniguruma), and the
# system Oniguruma ships only pkg-config metadata, so the Find module is what
# satisfies that check.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
find_package(QspLegacy 5.7.0 REQUIRED)

# Directly used Qt modules. Qt's imported targets carry their own transitive
# link dependencies, so lower-level Qt/system libraries are not listed here.
find_package(Qt6 6.2 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    LinguistTools
)

if(QQSP_ENABLE_WEBBOX)
    find_package(Qt6 6.2 REQUIRED COMPONENTS
        WebEngineCore
        WebEngineWidgets
        WebChannel
    )
endif()

set(QQSP_SOURCES
    src/app/main.cpp
    src/app/appbootstrap.cpp
    src/app/mainwindow.cpp
    src/app/rollbackmanager.cpp
    src/app/debuglogwindow.cpp
    src/widgets/qsptextbox.cpp
    src/widgets/qsplistbox.cpp
    src/widgets/qspinputbox.cpp
    src/widgets/qspimgcanvas.cpp
    src/dialogs/qspmsgdlg.cpp
    src/dialogs/qspinputdlg.cpp
    src/app/callbacks_gui.cpp
    src/app/comtools.cpp
    src/dialogs/optionsdialog.cpp
    src/multimedia/videolabel.cpp
)

set(QQSP_HEADERS
    src/app/appbootstrap.h
    src/app/mainwindow.h
    src/app/rollbackmanager.h
    src/app/debuglogwindow.h
    src/widgets/qsptextbox.h
    src/widgets/qsplistbox.h
    src/widgets/qspinputbox.h
    src/widgets/qspimgcanvas.h
    src/dialogs/qspmsgdlg.h
    src/dialogs/qspinputdlg.h
    src/app/callbacks_gui.h
    src/app/comtools.h
    src/dialogs/optionsdialog.h
    src/multimedia/videolabel.h
)

set(QQSP_FORMS
    src/dialogs/optionsdialog.ui
)

set(QQSP_RESOURCES
    qqsp.qrc
)

if(QQSP_ENABLE_WEBBOX)
    list(APPEND QQSP_SOURCES
        src/web/qspwebbox.cpp
        src/web/qspwebengineurlschemehandler.cpp
        src/web/qspexecwebengineurlschemehandler.cpp
        src/web/url_schemes.cpp
        src/web/qspwebchannel.cpp
    )

    list(APPEND QQSP_HEADERS
        src/web/qspwebbox.h
        src/web/qspwebengineurlschemehandler.h
        src/web/qspexecwebengineurlschemehandler.h
        src/web/url_schemes.h
        src/web/qspwebchannel.h
    )
endif()

set(QQSP_TRANSLATION_QM_FILES)
qt_add_translation(
    QQSP_TRANSLATION_QM_FILES
    Qqsp.en.ts
    Qqsp.ru.ts
)

set(QQSP_TRANSLATION_QRC "${CMAKE_CURRENT_BINARY_DIR}/qqsp-translations.qrc")
file(WRITE "${QQSP_TRANSLATION_QRC}" [=[
<RCC>
    <qresource prefix="/translations">
        <file>Qqsp.en.qm</file>
        <file>Qqsp.ru.qm</file>
    </qresource>
</RCC>
]=])

qt_add_resources(
    QQSP_TRANSLATION_RESOURCES
    "${QQSP_TRANSLATION_QRC}"
)

add_executable(Qqsp
    src/app/main.cpp
    ${QQSP_RESOURCES}
    ${QQSP_TRANSLATION_RESOURCES}
)

add_custom_target(QqspTranslations
    DEPENDS
        ${QQSP_TRANSLATION_QM_FILES}
)

add_dependencies(Qqsp QqspTranslations)

# Shared application sources as a static library: the executable and the
# gldriver smoke tool link the same objects instead of duplicating the
# source list (which had already drifted once).
set(QQSP_APP_SOURCES ${QQSP_SOURCES})
list(REMOVE_ITEM QQSP_APP_SOURCES src/app/main.cpp)

add_library(qqsp_app STATIC
    ${QQSP_APP_SOURCES}
    ${QQSP_HEADERS}
    ${QQSP_FORMS}
)

target_include_directories(qqsp_app
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/src/multimedia
        ${CMAKE_CURRENT_SOURCE_DIR}/src/web
)

target_compile_definitions(qqsp_app
    PUBLIC
        _UNICODE
        $<$<BOOL:${QQSP_ENABLE_WEBBOX}>:_WEBBOX>
    PRIVATE
        QT_DISABLE_DEPRECATED_UP_TO=${QQSP_QT_DISABLE_DEPRECATED}
)

target_link_libraries(qqsp_app
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Multimedia
        QspLegacy::qsp-legacy
)

if(QQSP_ENABLE_WEBBOX)
    target_link_libraries(qqsp_app
        PUBLIC
            Qt6::WebEngineWidgets
            Qt6::WebChannel
    )
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(qqsp_app PRIVATE _NONATIVEDIALOG)
endif()

target_include_directories(Qqsp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/src/multimedia
        ${CMAKE_CURRENT_SOURCE_DIR}/src/web
)

target_compile_definitions(Qqsp
    PRIVATE
        QT_DISABLE_DEPRECATED_UP_TO=${QQSP_QT_DISABLE_DEPRECATED}
        _UNICODE
)

# Disable native file dialogs on non-macOS Unix builds.
if(UNIX AND NOT APPLE)
    target_compile_definitions(Qqsp
        PRIVATE
            _NONATIVEDIALOG
    )
endif()

target_link_libraries(Qqsp
    PRIVATE
        qqsp_app
)



set_target_properties(Qqsp PROPERTIES
    OUTPUT_NAME Qqsp
)

# Preserve the application bundle/icon behavior on macOS.
if(APPLE)
    set_target_properties(Qqsp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE qsp-logo.icns
    )

    target_sources(Qqsp
        PRIVATE
            icons/qsp-logo.icns
    )

    set_source_files_properties(
        icons/qsp-logo.icns
        PROPERTIES
            MACOSX_PACKAGE_LOCATION Resources
    )
endif()

# Qqsp is a GUI subsystem application on Windows.
if(WIN32)
    set_target_properties(Qqsp PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()

# Regression tests (seed of ROADMAP M5). check.sh runs them through CTest.
enable_testing()

find_package(Qt6 6.2 REQUIRED COMPONENTS Gui Test)

add_executable(tst_paths
    tests/tst_paths.cpp
    src/app/comtools.cpp
    src/app/comtools.h
)

target_include_directories(tst_paths
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_link_libraries(tst_paths
    PRIVATE
        Qt6::Gui
        Qt6::Test
        QspLegacy::qsp-legacy
)

add_test(NAME tst_paths COMMAND tst_paths)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core)

add_executable(tst_engine
    tests/tst_engine.cpp
)

target_include_directories(tst_engine
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_compile_definitions(tst_engine
    PRIVATE
        QSP_TEST_FIXTURE="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/minimal.qsp"
)

target_link_libraries(tst_engine
    PRIVATE
        Qt6::Core
        Qt6::Test
        QspLegacy::qsp-legacy
)

add_test(NAME tst_engine COMMAND tst_engine)

find_package(Qt6 6.2 REQUIRED COMPONENTS Test)

add_executable(tst_rollback
    tests/tst_rollback.cpp
    src/app/rollbackmanager.cpp
    src/app/rollbackmanager.h
)

target_include_directories(tst_rollback
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
)

target_compile_definitions(tst_rollback
    PRIVATE
        QSP_TEST_FIXTURE="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures/minimal.qsp"
)

target_link_libraries(tst_rollback
    PRIVATE
        Qt6::Core
        Qt6::Test
        QspLegacy::qsp-legacy
)

add_test(NAME tst_rollback COMMAND tst_rollback)

# Headless smoke driver: runs the full client against a game file without
# interaction. A manual real-game reproducer (not part of CTest); the driver
# shares the application bootstrap (register_url_schemes +
# QspApp::configureApplication) with main.cpp.
add_executable(gldriver tests/gldriver.cpp)

target_compile_definitions(gldriver
    PRIVATE
        QT_DISABLE_DEPRECATED_UP_TO=${QQSP_QT_DISABLE_DEPRECATED}
        _NONATIVEDIALOG
)

target_link_libraries(gldriver
    PRIVATE
        qqsp_app
)
