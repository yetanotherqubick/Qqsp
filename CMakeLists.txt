cmake_minimum_required(VERSION 3.25)

project(Qqsp
    VERSION 1.9.0.0
    LANGUAGES C CXX
)

# Export the compilation database for clangd and other analysis tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set the language standards used by the application and bundled C code.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

# Qt code generation.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(QQSP_ENABLE_WEBBOX
    "Build the Qt WebEngine-based web box"
    ON
)

option(QQSP_ENABLE_WEBBOX_WEBKIT
    "Build the legacy Qt WebKit-based web box"
    OFF
)

option(QQSP_ENABLE_DEBUG
    "Enable the Qqsp memory-debugging support"
    OFF
)

option(QQSP_ENABLE_ANDROID
    "Enable Android-specific support"
    OFF
)

option(QQSP_IGNORE_QSP_INCOMPATIBLE_POINTER_WARNING
    "Ignore -Wincompatible-pointer-types warnings"
    ON
)

# Oniguruma is an external dependency located by cmake/Findoniguruma.cmake.
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Regular-expression support for the QSP engine. 6.9.10 is the final release of
# the archived upstream project; requiring it gives every build the same regex
# behavior and all upstream security fixes.
find_package(oniguruma 6.9.10 REQUIRED)

# Directly used Qt modules. Qt's imported targets carry their own transitive
# link dependencies, so lower-level Qt/system libraries are not listed here.
find_package(Qt5 5.15 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Multimedia
    LinguistTools
)

if(QQSP_ENABLE_WEBBOX)
    find_package(Qt5 5.15 REQUIRED COMPONENTS
        WebEngine
        WebEngineCore
        WebEngineWidgets
        WebChannel
    )
endif()

if(QQSP_ENABLE_WEBBOX_WEBKIT)
    find_package(Qt5 5.15 REQUIRED COMPONENTS
        Network
        WebKitWidgets
    )
endif()

if(QQSP_ENABLE_ANDROID)
    if(NOT ANDROID)
        message(FATAL_ERROR
            "QQSP_ENABLE_ANDROID requires an Android Qt build."
        )
    endif()

    find_package(Qt5 5.15 REQUIRED COMPONENTS
        AndroidExtras
    )
endif()

set(QQSP_SOURCES
    src/app/main.cpp
    src/app/mainwindow.cpp
    src/widgets/qsptextbox.cpp
    src/widgets/qsplistbox.cpp
    src/widgets/qspinputbox.cpp
    src/widgets/qspimgcanvas.cpp
    src/dialogs/qspmsgdlg.cpp
    src/dialogs/qspinputdlg.cpp
    src/app/callbacks_gui.cpp
    src/app/comtools.cpp
    src/dialogs/optionsdialog.cpp
    src/multimedia/videoframeprocessor.cpp
    src/multimedia/videolabel.cpp

    src/qsp/actions.c
    src/qsp/callbacks.c
    src/qsp/codetools.c
    src/qsp/coding.c
    src/qsp/common.c
    src/qsp/errors.c
    src/qsp/game.c
    src/qsp/locations.c
    src/qsp/mathops.c
    src/qsp/menu.c
    src/qsp/objects.c
    src/qsp/playlist.c
    src/qsp/statements.c
    src/qsp/text.c
    src/qsp/time_qsp.c
    src/qsp/towlower.c
    src/qsp/towupper.c
    src/qsp/variables.c
    src/qsp/variant.c

    src/qsp/bindings/default/default_callbacks.c
    src/qsp/bindings/default/default_coding.c
    src/qsp/bindings/default/default_control.c
)

set(QQSP_HEADERS
    src/app/mainwindow.h
    src/widgets/qsptextbox.h
    src/widgets/qsplistbox.h
    src/widgets/qspinputbox.h
    src/widgets/qspimgcanvas.h
    src/dialogs/qspmsgdlg.h
    src/dialogs/qspinputdlg.h
    src/app/callbacks_gui.h
    src/app/comtools.h
    src/dialogs/optionsdialog.h
    src/multimedia/videoframeprocessor.h
    src/multimedia/videolabel.h

    src/qsp/actions.h
    src/qsp/callbacks.h
    src/qsp/codetools.h
    src/qsp/coding.h
    src/qsp/common.h
    src/qsp/declarations.h
    src/qsp/errors.h
    src/qsp/game.h
    src/qsp/locations.h
    src/qsp/mathops.h
    src/qsp/menu.h
    src/qsp/objects.h
    src/qsp/playlist.h
    src/qsp/qsp.h
    src/qsp/statements.h
    src/qsp/text.h
    src/qsp/time_qsp.h
    src/qsp/variables.h
    src/qsp/variant.h

    src/qsp/bindings/bindings_config.h
    src/qsp/bindings/default/qsp_default.h
)

set(QQSP_FORMS
    src/dialogs/optionsdialog.ui
)

set(QQSP_RESOURCES
    qqsp.qrc
)

if(QQSP_ENABLE_DEBUG)
    list(APPEND QQSP_SOURCES
        src/qsp/memwatch.c
    )

    list(APPEND QQSP_HEADERS
        src/qsp/memwatch.h
    )
endif()

if(QQSP_ENABLE_WEBBOX)
    list(APPEND QQSP_SOURCES
        src/web/qspwebbox.cpp
        src/web/qspwebengineurlrequestinterceptor.cpp
        src/web/qspwebengineurlschemehandler.cpp
        src/web/qspexecwebengineurlschemehandler.cpp
        src/web/url_schemes.cpp
        src/web/qspwebchannel.cpp
    )

    list(APPEND QQSP_HEADERS
        src/web/qspwebbox.h
        src/web/qspwebengineurlrequestinterceptor.h
        src/web/qspwebengineurlschemehandler.h
        src/web/qspexecwebengineurlschemehandler.h
        src/web/url_schemes.h
        src/web/qspwebchannel.h
    )
endif()

if(QQSP_ENABLE_WEBBOX_WEBKIT)
    list(APPEND QQSP_SOURCES
        src/web/qspwebbox_webkit.cpp
        src/web/qspnetworkaccessmanager.cpp
        src/web/qspreply.cpp
    )

    list(APPEND QQSP_HEADERS
        src/web/qspwebbox_webkit.h
        src/web/qspnetworkaccessmanager.h
        src/web/qspreply.h
    )
endif()

if(QQSP_ENABLE_ANDROID)
    list(APPEND QQSP_SOURCES
        src/platform/android/androidfiledialog.cpp
    )

    list(APPEND QQSP_HEADERS
        src/platform/android/androidfiledialog.h
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
    ${QQSP_SOURCES}
    ${QQSP_HEADERS}
    ${QQSP_FORMS}
    ${QQSP_RESOURCES}
    ${QQSP_TRANSLATION_RESOURCES}
)

add_custom_target(QqspTranslations
    DEPENDS
        ${QQSP_TRANSLATION_QM_FILES}
)

add_dependencies(Qqsp QqspTranslations)

if(QQSP_IGNORE_QSP_INCOMPATIBLE_POINTER_WARNING)
    target_compile_options(Qqsp
        PRIVATE
            $<$<COMPILE_LANGUAGE:C>:-Wno-incompatible-pointer-types>
    )
endif()

target_include_directories(Qqsp
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/app
        ${CMAKE_CURRENT_SOURCE_DIR}/src/dialogs
        ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
        ${CMAKE_CURRENT_SOURCE_DIR}/src/multimedia
        ${CMAKE_CURRENT_SOURCE_DIR}/src/web
        ${CMAKE_CURRENT_SOURCE_DIR}/src/platform/android
        ${CMAKE_CURRENT_SOURCE_DIR}/src/qsp/bindings/default
)

target_compile_definitions(Qqsp
    PRIVATE
        QT_DISABLE_DEPRECATED_UP_TO=0x050F00
        _UNICODE
)

if(QQSP_ENABLE_DEBUG)
    target_compile_definitions(Qqsp
        PRIVATE
            _DEBUGMW
    )
endif()

if(QQSP_ENABLE_WEBBOX)
    target_compile_definitions(Qqsp
        PRIVATE
            _WEBBOX
            _WEBBOX_COMMON
    )
endif()

if(QQSP_ENABLE_WEBBOX_WEBKIT)
    target_compile_definitions(Qqsp
        PRIVATE
            _WEBBOX_WEBKIT
            _WEBBOX_COMMON
    )
endif()

if(QQSP_ENABLE_ANDROID)
    target_compile_definitions(Qqsp
        PRIVATE
            _ANDROIDQT
    )
endif()

# Disable native file dialogs on non-macOS Unix builds.
if(UNIX AND NOT APPLE)
    target_compile_definitions(Qqsp
        PRIVATE
            _NONATIVEDIALOG
    )
endif()

target_link_libraries(Qqsp
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Multimedia
        oniguruma::onig
)

if(QQSP_ENABLE_WEBBOX)
    target_link_libraries(Qqsp
        PRIVATE
            Qt::WebEngine
            Qt::WebEngineCore
            Qt::WebEngineWidgets
            Qt::WebChannel
    )
endif()

if(QQSP_ENABLE_WEBBOX_WEBKIT)
    target_link_libraries(Qqsp
        PRIVATE
            Qt::Network
            Qt::WebKitWidgets
    )
endif()

if(QQSP_ENABLE_ANDROID)
    target_link_libraries(Qqsp
        PRIVATE
            Qt::AndroidExtras
    )
endif()

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
