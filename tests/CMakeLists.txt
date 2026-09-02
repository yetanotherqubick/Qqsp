add_executable(test_comtools
    unit/test_comtools.cpp
    ${CMAKE_SOURCE_DIR}/src/application/comtools.cpp
)

target_include_directories(test_comtools PRIVATE
    ${CMAKE_SOURCE_DIR}/src/application
    ${CMAKE_SOURCE_DIR}/qsp/bindings/default
)

target_compile_definitions(test_comtools PRIVATE _UNICODE)
target_link_libraries(test_comtools PRIVATE Qt5::Test Qt5::Gui Qt5::Core)
set_target_properties(test_comtools PROPERTIES AUTOMOC ON)

add_test(NAME comtools COMMAND test_comtools)

add_executable(test_textbox
    unit/test_textbox.cpp
    ${CMAKE_SOURCE_DIR}/src/widgets/textbox.cpp
    ${CMAKE_SOURCE_DIR}/src/application/comtools.cpp
    ${CMAKE_SOURCE_DIR}/src/media/videolabel.cpp
    ${CMAKE_SOURCE_DIR}/src/media/videoframeprocessor.cpp
)

target_include_directories(test_textbox PRIVATE
    ${CMAKE_SOURCE_DIR}/src/application
    ${CMAKE_SOURCE_DIR}/src/widgets
    ${CMAKE_SOURCE_DIR}/src/media
    ${CMAKE_SOURCE_DIR}/qsp/bindings/default
)

target_compile_definitions(test_textbox PRIVATE _UNICODE)
target_link_libraries(test_textbox PRIVATE
    Qt5::Test Qt5::Widgets Qt5::Multimedia Qt5::Gui Qt5::Core)
set_target_properties(test_textbox PROPERTIES AUTOMOC ON)

add_test(NAME textbox COMMAND test_textbox)
set_tests_properties(textbox PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

add_executable(test_videoframeprocessor
    unit/test_videoframeprocessor.cpp
    ${CMAKE_SOURCE_DIR}/src/media/videoframeprocessor.cpp
)

target_link_libraries(test_videoframeprocessor PRIVATE
    Qt5::Test Qt5::Multimedia Qt5::Gui Qt5::Core)
set_target_properties(test_videoframeprocessor PROPERTIES AUTOMOC ON)

add_test(NAME videoframeprocessor COMMAND test_videoframeprocessor)
