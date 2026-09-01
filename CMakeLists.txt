cmake_minimum_required(VERSION 3.16)

project(Qqsp VERSION 1.9.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(QQSP_ENABLE_WEBBOX "Build the Qt WebEngine web box" ON)
option(QQSP_ENABLE_DEBUG "Enable Qqsp memory debugging" OFF)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Multimedia)
if(QQSP_ENABLE_WEBBOX)
    find_package(Qt5 REQUIRED COMPONENTS WebChannel WebEngine WebEngineWidgets)
endif()

set(QQSP_SOURCES
    main.cpp mainwindow.cpp qsptextbox.cpp qsplistbox.cpp qspinputbox.cpp
    qspimgcanvas.cpp qspmsgdlg.cpp qspinputdlg.cpp callbacks_gui.cpp
    comtools.cpp optionsdialog.cpp videoframeprocessor.cpp videolabel.cpp
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

set(QQSP_HEADERS
    mainwindow.h qsptextbox.h qsplistbox.h qspinputbox.h qspimgcanvas.h
    qspmsgdlg.h qspinputdlg.h callbacks_gui.h comtools.h optionsdialog.h
    videoframeprocessor.h videolabel.h qsp/actions.h qsp/callbacks.h
    qsp/codetools.h qsp/coding.h qsp/common.h qsp/declarations.h qsp/errors.h
    qsp/game.h qsp/locations.h qsp/mathops.h qsp/menu.h qsp/objects.h
    qsp/playlist.h qsp/qsp.h qsp/statements.h qsp/text.h qsp/time_qsp.h
    qsp/variables.h qsp/variant.h qsp/onig/config.h qsp/onig/oniguruma.h
    qsp/onig/regenc.h qsp/onig/regint.h qsp/onig/regparse.h qsp/onig/st.h
    qsp/bindings/bindings_config.h qsp/bindings/default/qsp_default.h
)

if(QQSP_ENABLE_DEBUG)
    list(APPEND QQSP_SOURCES qsp/memwatch.c)
    list(APPEND QQSP_HEADERS qsp/memwatch.h)
endif()

if(QQSP_ENABLE_WEBBOX)
    list(APPEND QQSP_SOURCES
        qspwebbox.cpp qspwebengineurlrequestinterceptor.cpp
        qspwebengineurlschemehandler.cpp qspexecwebengineurlschemehandler.cpp
        url_schemes.cpp qspwebchannel.cpp)
    list(APPEND QQSP_HEADERS
        qspwebbox.h qspwebengineurlrequestinterceptor.h
        qspwebengineurlschemehandler.h qspexecwebengineurlschemehandler.h
        url_schemes.h qspwebchannel.h)
endif()

add_executable(Qqsp ${QQSP_SOURCES} ${QQSP_HEADERS} optionsdialog.ui qqsp.qrc)
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
    ${CMAKE_CURRENT_SOURCE_DIR}/qsp/bindings/default)

target_link_libraries(Qqsp PRIVATE
    Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Multimedia)
if(QQSP_ENABLE_WEBBOX)
    target_link_libraries(Qqsp PRIVATE Qt5::WebChannel Qt5::WebEngine Qt5::WebEngineWidgets)
endif()

if(WIN32)
    target_sources(Qqsp PRIVATE Qqsp.rc)
endif()

if(APPLE)
    set_target_properties(Qqsp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME Qqsp
        MACOSX_BUNDLE_GUI_IDENTIFIER org.qsp.Qqsp
        MACOSX_BUNDLE_ICON_FILE qsp-logo.icns)
    set_source_files_properties(icons/qsp-logo.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION Resources)
    target_sources(Qqsp PRIVATE icons/qsp-logo.icns)
endif()

include(GNUInstallDirs)
install(TARGETS Qqsp
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
