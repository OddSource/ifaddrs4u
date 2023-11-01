#[[
    Copyright © 2010-2023 OddSource Code (license@oddsource.io)

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
]]
cmake_minimum_required(VERSION 3.22)

project(ifaddrs4cpp
        VERSION 0.1.0
        DESCRIPTION "A C++17 library for cross-platform access to network interfaces and their addresses"
        HOMEPAGE_URL "https://github.com/OddSource/ifaddrs4u"
        LANGUAGES CXX)
set(PROJECT_VERSION_SUFFIX "alpha0")

option(ENABLE_TESTS
       "If enabled, tests will be build and prepared for running with CTest or ifaddrs4cpp_tests."
        False)
option(BUILD_STATIC_ONLY
       "If enabled, only a static library will be built, otherwise both static and dynamic will be built."
       False)
option(BUILD_DYNAMIC_ONLY
       "If enabled, only a dynamic library will be built, otherwise both static and dynamic will be built."
       False)

if(${BUILD_STATIC_ONLY} AND ${BUILD_DYNAMIC_ONLY})
    message(FATAL_ERROR
            "Options BUILD_STATIC_ONLY and BUILD_DYNAMIC_ONLY are mutually exclusive. Only one may be specified at a time.")
endif()
if(${BUILD_DYNAMIC_ONLY} AND ${ENABLE_TESTS})
    message(FATAL_ERROR
            "Options ENABLE_TESTS and BUILD_DYNAMIC_ONLY are mutually exclusive. Only one may be specified at a time.")
endif()

execute_process(COMMAND git log -1 --format=%H
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                OUTPUT_VARIABLE PROJECT_GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND git log -1 --format=%h
                WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
                OUTPUT_VARIABLE PROJECT_GIT_HASH_SHORT
                OUTPUT_STRIP_TRAILING_WHITESPACE)

set(${PROJECT_NAME}_VERSION_SUFFIX "${PROJECT_VERSION_SUFFIX}")
set(${PROJECT_NAME}_GIT_HASH "${PROJECT_GIT_HASH}")
set(${PROJECT_NAME}_GIT_HASH_SHORT "${PROJECT_GIT_HASH_SHORT}")

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(gcc_like_ids ARMClang AppleClang Clang GNU LCC)
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(IS_WINDOWS FALSE)
set(COPY_COMMAND "cp")
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    set(IS_WINDOWS TRUE)
    set(COPY_COMMAND "copy")
endif()

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_DEBUG_POSTFIX -d)
if(CMAKE_CXX_COMPILER_ID IN_LIST gcc_like_ids)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

set(${PROJECT_NAME}_sources
    Interface.cpp
    Interfaces.cpp
    IpAddress.cpp
    MacAddress.cpp)

configure_file(version.h.in version.h)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}_compiler_flags INTERFACE
                       "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wdeprecated;-Wextra;-Wpedantic;-Wshadow;-Wunused;-Werror>>"
                       "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3;-wd4275;-wd4251;-wd4455>>")
set_target_properties(${PROJECT_NAME}_compiler_flags PROPERTIES
                      DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS OFF)

# Windows compiler warnings disablement explanations:
# - C4275: Docs say this can be ignored if your class is derived from a type in the C++ Standard
# - C4251: Docs say this can be ignored if your class is exposing a type in the C++ Standard
# - C4455: "literal suffix identifiers that do not start with an underscore" when using std ""s

set(HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET ${PROJECT_NAME}-shared)

if(NOT ${BUILD_STATIC_ONLY})
    add_library(${PROJECT_NAME}-shared SHARED ${${PROJECT_NAME}_sources})
    target_link_libraries(${PROJECT_NAME}-shared PUBLIC ${PROJECT_NAME}_compiler_flags)
    target_include_directories(${PROJECT_NAME}-shared PUBLIC "${PROJECT_BINARY_DIR}")
    if(${IS_WINDOWS})
        target_compile_definitions(${PROJECT_NAME}-shared PUBLIC ODDSOURCE_BUILDING_LIBRARY=1)
    endif()
endif()

if(NOT ${BUILD_DYNAMIC_ONLY})
    set(HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET ${PROJECT_NAME}-static)
    add_library(${PROJECT_NAME}-static STATIC ${${PROJECT_NAME}_sources})
    target_link_libraries(${PROJECT_NAME}-static PUBLIC ${PROJECT_NAME}_compiler_flags)
    target_include_directories(${PROJECT_NAME}-static PUBLIC "${PROJECT_BINARY_DIR}")
    if(${IS_WINDOWS})
        target_compile_definitions(${PROJECT_NAME}-static PUBLIC ODDSOURCE_BUILDING_LIBRARY=1)
    endif()
    set_target_properties(${PROJECT_NAME}-static PROPERTIES
                          POSITION_INDEPENDENT_CODE ON)
endif()

if(${IS_WINDOWS})
    add_custom_command(TARGET ${HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET} POST_BUILD COMMAND
                       mkdir ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
else()
    add_custom_command(TARGET ${HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET} POST_BUILD COMMAND
                       mkdir -p ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
endif()
add_custom_command(TARGET ${HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET} POST_BUILD COMMAND
                   ${COPY_COMMAND} ${CMAKE_BINARY_DIR}/version.h ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
add_custom_command(TARGET ${HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET} POST_BUILD COMMAND
                   ${COPY_COMMAND} ${CMAKE_SOURCE_DIR}/*.h ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
add_custom_command(TARGET ${HEADER_ARCHIVE_CUSTOM_COMMAND_TARGET} POST_BUILD COMMAND
                   ${COPY_COMMAND} ${CMAKE_SOURCE_DIR}/*.hpp ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)

if (${ENABLE_TESTS})
    add_executable(${PROJECT_NAME}_tests
                   tests/main.cpp
                   tests/TestIPv4Address.cpp
                   tests/TestIPv6Address.cpp
                   tests/TestMacAddress.cpp
                   tests/TestInterface.cpp
                   tests/TestInterfaceIPAddress.cpp
                   tests/TestVersionInfo.cpp
                   tests/TestInterfaceBrowser.cpp)
    target_link_libraries(${PROJECT_NAME}_tests PUBLIC ${PROJECT_NAME}_compiler_flags)
    target_link_libraries(${PROJECT_NAME}_tests PUBLIC ${PROJECT_NAME}-static)
    target_include_directories(${PROJECT_NAME}_tests PUBLIC "${PROJECT_BINARY_DIR}")

    enable_testing()
    add_test(NAME Tests COMMAND ${PROJECT_NAME}_tests)
endif()
