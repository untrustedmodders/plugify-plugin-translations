cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()


file(READ "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION_FILE_CONTENTS)
string(STRIP "${VERSION_FILE_CONTENTS}" VERSION_FILE_CONTENTS)
set(TRANSLATIONS_VERSION "${VERSION_FILE_CONTENTS}" CACHE STRING "Set version name")
set(TRANSLATIONS_PACKAGE "translations" CACHE STRING "Set package name")
string(REPLACE "v" "" TRANSLATIONS_VERSION "${TRANSLATIONS_VERSION}")
string(REGEX REPLACE "[.+-]" ";" TRANSLATIONS_VERSION_LIST ${TRANSLATIONS_VERSION})
list(GET TRANSLATIONS_VERSION_LIST 0 TRANSLATIONS_VERSION_MAJOR)
list(GET TRANSLATIONS_VERSION_LIST 1 TRANSLATIONS_VERSION_MINOR)
list(GET TRANSLATIONS_VERSION_LIST 2 TRANSLATIONS_VERSION_PATCH)

project(plugify-plugin-translations
        VERSION "${TRANSLATIONS_VERSION_MAJOR}.${TRANSLATIONS_VERSION_MINOR}.${TRANSLATIONS_VERSION_PATCH}"
        DESCRIPTION "Translations Plugin" 
        HOMEPAGE_URL "https://github.com/untrustedmodders/plugify-plugin-translations" 
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE MATCHES "Debug|Devel|MinSizeRel|RelWithDebInfo|Release")
    message(STATUS "CMAKE_BUILD_TYPE not set, defaulting to Release.")
    set(CMAKE_BUILD_TYPE Release)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

#
# Fmt
#
include(FetchFmt)

#
# Plugin
#
file(GLOB_RECURSE PLUGIN_SOURCES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "src/*.cpp")

add_library(${PROJECT_NAME} SHARED ${PLUGIN_SOURCES}
        "${CMAKE_CURRENT_SOURCE_DIR}/external/plugify/include/configs/configs.cpp")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/external/plugify/include")

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX /Zc:preprocessor $<$<CONFIG:Release>:/Zi>)
    target_link_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:/DEBUG /OPT:REF /OPT:ICF>)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wextra -Wshadow -Wconversion -Wpedantic -Werror $<$<CONFIG:Release>:-g1>)
endif()

if(LINUX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _GLIBCXX_USE_CXX11_ABI=1)
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libstdc++ -static-libgcc)
    #target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>: -fsanitize=address -fsanitize=leak -fsanitize=undefined>)
    #target_link_libraries(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>: -fsanitize=address -fsanitize=leak -fsanitize=undefined>)
endif()

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/sym/exported_symbols.lds")
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME PLUGIFY_API EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/exports/plugify_export.h")
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_BINARY_DIR}/exports")

target_compile_definitions(${PROJECT_NAME} PRIVATE 
        PLUGIFY_FORMAT_SUPPORT=$<BOOL:${COMPILER_SUPPORTS_FORMAT}>
        PLUGIFY_IS_DEBUG=$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>
        PLUGIFY_IS_RELEASE=$<STREQUAL:${CMAKE_BUILD_TYPE},Release>
        PLUGIFY_MODULE="${PROJECT_NAME}"
)
if(TRUE)#NOT COMPILER_SUPPORTS_FORMAT)
    # only if format is missing
    target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
endif()

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/plugify-plugin-translations.pplugin.in
    ${CMAKE_CURRENT_BINARY_DIR}/plugify-plugin-translations.pplugin
)

find_program(CLANG_DOC clang-doc)
if(CLANG_DOC)
    add_custom_target(docs
            COMMAND ${CLANG_DOC}
            --executor=all-TUs
            -p ${CMAKE_CURRENT_BINARY_DIR}
            --output=${CMAKE_CURRENT_SOURCE_DIR}/docs
            --extra-arg=-Wno-error
            --format=yaml
            ${CMAKE_SOURCE_DIR}/src/*.cpp
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Generating documentation with clang-doc"
    )
endif()