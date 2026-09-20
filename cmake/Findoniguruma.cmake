# Findoniguruma
# -------------
#
# Find the Oniguruma regular expression library.
#
# Written for Qqsp. The lookup order and the oniguruma::onig target name follow
# the Findoniguruma.cmake found in QSPFoundation/qsp; the rest is new.
#
# Imported target:
#
#   oniguruma::onig
#
# Result variables:
#
#   oniguruma_FOUND    - the library was found in an acceptable version
#   oniguruma_VERSION  - the version that was found
#
# A version given to find_package() is a minimum version. Sources are tried in
# this order, and the first acceptable one wins:
#
#   1. the CMake package configuration installed by Oniguruma or a package
#      manager (for example vcpkg)
#   2. pkg-config
#   3. a plain header and library search (CMAKE_PREFIX_PATH is honored)

if(NOT TARGET oniguruma::onig)
    # A nested find_package(oniguruma) may reset the outer request variables.
    set(_oniguruma_min_version "${oniguruma_FIND_VERSION}")

    # 1. CMake package configuration.
    find_package(oniguruma ${_oniguruma_min_version} CONFIG QUIET)
    if(oniguruma_FOUND AND TARGET onig AND NOT TARGET oniguruma::onig)
        add_library(oniguruma::onig ALIAS onig)
    endif()

    # 2. pkg-config.
    if(NOT TARGET oniguruma::onig)
        find_package(PkgConfig QUIET)
        if(PKG_CONFIG_FOUND)
            set(_oniguruma_pc_spec oniguruma)
            if(_oniguruma_min_version)
                set(_oniguruma_pc_spec "oniguruma>=${_oniguruma_min_version}")
            endif()
            pkg_check_modules(PC_oniguruma QUIET IMPORTED_TARGET GLOBAL
                "${_oniguruma_pc_spec}"
            )
            if(PC_oniguruma_FOUND)
                add_library(oniguruma::onig INTERFACE IMPORTED GLOBAL)
                target_link_libraries(oniguruma::onig
                    INTERFACE PkgConfig::PC_oniguruma
                )
                set(oniguruma_VERSION "${PC_oniguruma_VERSION}")
            endif()
        endif()
    endif()

    # 3. Plain header and library search. This cannot tell a static library from
    # an import library: upstream documents that a static Windows build also
    # needs ONIG_EXTERN=extern on the compiler command line.
    if(NOT TARGET oniguruma::onig)
        find_path(oniguruma_INCLUDE_DIR NAMES oniguruma.h)
        find_library(oniguruma_LIBRARY NAMES onig)
        mark_as_advanced(oniguruma_INCLUDE_DIR oniguruma_LIBRARY)

        if(oniguruma_INCLUDE_DIR AND oniguruma_LIBRARY)
            file(STRINGS "${oniguruma_INCLUDE_DIR}/oniguruma.h"
                _oniguruma_version_lines
                REGEX "^#define[ \t]+ONIGURUMA_VERSION_(MAJOR|MINOR|TEENY)[ \t]+[0-9]+"
            )
            set(oniguruma_VERSION "")
            set(_oniguruma_version_parts "")
            foreach(_part MAJOR MINOR TEENY)
                if(_oniguruma_version_lines
                        MATCHES "ONIGURUMA_VERSION_${_part}[ \t]+([0-9]+)")
                    list(APPEND _oniguruma_version_parts "${CMAKE_MATCH_1}")
                endif()
            endforeach()
            list(LENGTH _oniguruma_version_parts _oniguruma_part_count)
            if(_oniguruma_part_count EQUAL 3)
                list(JOIN _oniguruma_version_parts "." oniguruma_VERSION)
            endif()

            if(NOT _oniguruma_min_version
                    OR (oniguruma_VERSION
                        AND NOT oniguruma_VERSION VERSION_LESS _oniguruma_min_version))
                add_library(oniguruma::onig UNKNOWN IMPORTED)
                set_target_properties(oniguruma::onig PROPERTIES
                    INTERFACE_INCLUDE_DIRECTORIES "${oniguruma_INCLUDE_DIR}"
                    IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                    IMPORTED_LOCATION "${oniguruma_LIBRARY}"
                )
            endif()
        endif()
    endif()
endif()

if(TARGET oniguruma::onig)
    set(oniguruma_TARGET oniguruma::onig)
else()
    set(oniguruma_TARGET oniguruma_TARGET-NOTFOUND)
endif()

include(FindPackageHandleStandardArgs)
if(oniguruma_VERSION)
    find_package_handle_standard_args(oniguruma
        REQUIRED_VARS oniguruma_TARGET
        VERSION_VAR oniguruma_VERSION
    )
else()
    find_package_handle_standard_args(oniguruma
        REQUIRED_VARS oniguruma_TARGET
    )
endif()
