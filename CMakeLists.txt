cmake_minimum_required(VERSION 3.16)

project(Qqsp VERSION 1.9.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(QQSP_ENABLE_WEBBOX "Build the Qt WebEngine web box" ON)
option(QQSP_ENABLE_DEBUG "Enable Qqsp memory debugging" OFF)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia Test)
if(QQSP_ENABLE_WEBBOX)
    find_package(Qt5 REQUIRED COMPONENTS WebChannel WebEngine WebEngineWidgets)
endif()

set(QQSP_APPLICATION_SOURCES
    src/application/main.cpp
    src/application/mainwindow.cpp
    src/application/callbacks_gui.cpp
    src/application/comtools.cpp
    src/application/optionsdialog.cpp
)

set(QQSP_APPLICATION_HEADERS
    src/application/mainwindow.h
    src/application/callbacks_gui.h
    src/application/comtools.h
    src/application/optionsdialog.h
    src/application/optionsdialog.ui
)

set(QQSP_WIDGET_SOURCES
    src/widgets/textbox.cpp
    src/widgets/listbox.cpp
    src/widgets/inputbox.cpp
    src/widgets/inputdlg.cpp
    src/widgets/msgdlg.cpp
    src/widgets/imgcanvas.cpp
)

set(QQSP_WIDGET_HEADERS
    src/widgets/qsptextbox.h
    src/widgets/qsplistbox.h
    src/widgets/qspinputbox.h
    src/widgets/qspinputdlg.h
    src/widgets/qspmsgdlg.h
    src/widgets/qspimgcanvas.h
)

set(QQSP_MEDIA_SOURCES
    src/media/videoframeprocessor.cpp
    src/media/videolabel.cpp
)

set(QQSP_MEDIA_HEADERS
    src/media/videoframeprocessor.h
    src/media/videolabel.h
)

set(QQSP_WEB_SOURCES
    src/web/networkaccessmanager.cpp
    src/web/reply.cpp
    src/web/webbox.cpp
    src/web/webengineurlrequestinterceptor.cpp
    src/web/webengineurlschemehandler.cpp
    src/web/execwebengineurlschemehandler.cpp
    src/web/webchannel.cpp
    src/web/url_schemes.cpp
)

set(QQSP_WEB_HEADERS
    src/web/qspnetworkaccessmanager.h
    src/web/qspreply.h
    src/web/qspwebbox.h
    src/web/qspwebengineurlrequestinterceptor.h
    src/web/qspwebengineurlschemehandler.h
    src/web/qspexecwebengineurlschemehandler.h
    src/web/qspwebchannel.h
    src/web/url_schemes.h
)

set(QQSP_ENGINE_SOURCES
    qsp/actions.c qsp/callbacks.c qsp/codetools.c qsp/coding.c qsp/common.c
    qsp/errors.c qsp/game.c qsp/locations.c qsp/mathops.c qsp/menu.c
    qsp/objects.c qsp/playlist.c qsp/statements.c qsp/text.c qsp/time_qsp.c
    qsp/towlower.c qsp/towupper.c qsp/variables.c qsp/variant.c
    qsp/onig/regcomp.c qsp/onig/regenc.c qsp/onig/regerror.c qsp/onig/regexec.c
    qsp/onig/regparse.c qsp/onig/regsyntax.c qsp/onig/regtrav.c
    qsp/onig/regversion.c qsp/onig/st.c qsp/onig/enc/ascii.c
    qsp/onig/enc/cp1251.c qsp/onig/enc/koi8_r.c qsp/onig/enc/unicode.c
    qsp/onig/enc/utf16_be.c qsp/onig/enc/utf16_le.c qsp/onig/enc/utf32_be.c
    qsp/onig/enc/utf32_le.c qsp/bindings/default/default_callbacks.c
    qsp/bindings/default/default_coding.c qsp/bindings/default/default_control.c
)

set(QQSP_ENGINE_HEADERS
    qsp/actions.h qsp/callbacks.h qsp/codetools.h qsp/coding.h qsp/common.h
    qsp/declarations.h qsp/errors.h qsp/game.h qsp/locations.h qsp/mathops.h
    qsp/menu.h qsp/objects.h qsp/playlist.h qsp/qsp.h qsp/statements.h
    qsp/text.h qsp/time_qsp.h qsp/variables.h qsp/variant.h qsp/onig/config.h
    qsp/onig/oniguruma.h qsp/onig/regenc.h qsp/onig/regint.h qsp/onig/regparse.h
    qsp/onig/st.h qsp/bindings/bindings_config.h qsp/bindings/default/qsp_default.h
)

set(QQSP_SOURCES
    ${QQSP_APPLICATION_SOURCES}
    ${QQSP_WIDGET_SOURCES}
    ${QQSP_MEDIA_SOURCES}
    ${QQSP_ENGINE_SOURCES}
)

set(QQSP_HEADERS
    ${QQSP_APPLICATION_HEADERS}
    ${QQSP_WIDGET_HEADERS}
    ${QQSP_MEDIA_HEADERS}
    ${QQSP_ENGINE_HEADERS}
)

if(QQSP_ENABLE_DEBUG)
    list(APPEND QQSP_SOURCES qsp/memwatch.c)
    list(APPEND QQSP_HEADERS qsp/memwatch.h)
endif()

if(QQSP_ENABLE_WEBBOX)
    list(APPEND QQSP_SOURCES ${QQSP_WEB_SOURCES})
    list(APPEND QQSP_HEADERS ${QQSP_WEB_HEADERS})
endif()

add_executable(Qqsp
    ${QQSP_SOURCES}
    ${QQSP_HEADERS}
    src/resources/qqsp.qrc
)
set_target_properties(Qqsp PROPERTIES AUTOMOC ON AUTOUIC ON AUTORCC ON)

target_compile_definitions(Qqsp PRIVATE
    QT_DEPRECATED_WARNINGS
    QT_DISABLE_DEPRECATED_BEFORE=0x050F00
    _UNICODE NOT_RUBY)
if(QQSP_ENABLE_DEBUG)
    target_compile_definitions(Qqsp PRIVATE _DEBUGMW)
endif()
if(QQSP_ENABLE_WEBBOX)
    target_compile_definitions(Qqsp PRIVATE _WEBBOX _WEBBOX_COMMON)
endif()
if(NOT WIN32 AND NOT APPLE)
    target_compile_definitions(Qqsp PRIVATE _NONATIVEDIALOG)
endif()

target_include_directories(Qqsp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/application
    ${CMAKE_CURRENT_SOURCE_DIR}/src/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}/src/media
    ${CMAKE_CURRENT_SOURCE_DIR}/src/web
    ${CMAKE_CURRENT_SOURCE_DIR}/qsp/bindings/default)

target_link_libraries(Qqsp PRIVATE
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)
if(QQSP_ENABLE_WEBBOX)
    target_link_libraries(Qqsp PRIVATE Qt5::WebChannel Qt5::WebEngine Qt5::WebEngineWidgets)
endif()

if(WIN32)
    target_sources(Qqsp PRIVATE packaging/windows/Qqsp.rc)
endif()

if(APPLE)
    set_target_properties(Qqsp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME Qqsp
        MACOSX_BUNDLE_GUI_IDENTIFIER org.qsp.Qqsp
        MACOSX_BUNDLE_ICON_FILE qsp-logo.icns)
    set_source_files_properties(src/resources/icons/qsp-logo.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    target_sources(Qqsp PRIVATE src/resources/icons/qsp-logo.icns)
endif()

include(GNUInstallDirs)
install(TARGETS Qqsp
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

enable_testing()
add_subdirectory(tests)
