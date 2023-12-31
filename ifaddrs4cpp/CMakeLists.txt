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
option(ENABLE_ADDRESS_SANITIZER
       "If enabled, Address Sanitizer (ASAN) will be enabled on macOS and Linux, and also Leak Sanitizer (LSAN) on Linux"
       False)
option(SHARED_ADDRESS_SANITIZER
       "If enabled with ENABLE_ADDRESS_SANITIZER, the -shared-libsan option will be used (ignored without ENABLE_ADDRESS_SANITIZER)"
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

set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_DEBUG_POSTFIX -d)
if(CMAKE_CXX_COMPILER_ID IN_LIST gcc_like_ids)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "The macOS target" FORCE)

if(${ENABLE_ADDRESS_SANITIZER})
    if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    else()
        message(FATAL_ERROR
                "Option ENABLE_ADDRESS_SANITIZER requires configuration Debug.")
    endif()

    if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    else()
        add_compile_options(-fsanitize=address -fsanitize-address-use-after-scope)
        add_link_options(-fsanitize=address -fsanitize-address-use-after-scope)
        if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
            add_compile_options(-fsanitize-address-use-odr-indicator)
            add_link_options(-fsanitize-address-use-odr-indicator)
            if(${SHARED_ADDRESS_SANITIZER})
                add_compile_options(-shared-libsan)
            endif()
        elseif(!${SHARED_ADDRESS_SANITIZER})
            add_compile_options(-static-libasan)
        endif()
    endif()
endif()

set(${PROJECT_NAME}_sources
    Interface.cpp
    Interfaces.cpp
    IpAddress.cpp
    MacAddress.cpp)

configure_file(version.h.in version.h)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
file(COPY ${CMAKE_BINARY_DIR}/version.h DESTINATION ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
file(GLOB PUBLIC_HEADERS_TO_COPY ${CMAKE_SOURCE_DIR}/*.h)
file(COPY ${PUBLIC_HEADERS_TO_COPY} DESTINATION ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)
file(GLOB PUBLIC_HEADERS_TO_COPY ${CMAKE_SOURCE_DIR}/*.hpp)
file(COPY ${PUBLIC_HEADERS_TO_COPY} DESTINATION ${CMAKE_BINARY_DIR}/include/ifaddrs4cpp)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}_compiler_flags INTERFACE
                       "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wdeprecated;-Wextra;-Wpedantic;-Wshadow;-Wunused;-Werror>>"
                       "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3;-wd4275;-wd4251;-wd4455>>")
target_compile_definitions(${PROJECT_NAME}_compiler_flags INTERFACE ODDSOURCE_BUILDING_LIBRARY=1)
set_target_properties(${PROJECT_NAME}_compiler_flags PROPERTIES
                      DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS OFF)

# Windows compiler warnings disablement explanations:
# - C4275: Docs say this can be ignored if your class is derived from a type in the C++ Standard
# - C4251: Docs say this can be ignored if your class is exposing a type in the C++ Standard
# - C4455: "literal suffix identifiers that do not start with an underscore" when using std ""s

if(NOT ${BUILD_STATIC_ONLY})
    add_library(${PROJECT_NAME}-shared SHARED ${${PROJECT_NAME}_sources})
    target_link_libraries(${PROJECT_NAME}-shared PUBLIC ${PROJECT_NAME}_compiler_flags)
    target_include_directories(${PROJECT_NAME}-shared PUBLIC "${PROJECT_BINARY_DIR}")
    set_target_properties(${PROJECT_NAME}-shared PROPERTIES
                          CXX_VISIBILITY_PRESET hidden)
endif()

if(NOT ${BUILD_DYNAMIC_ONLY})
    add_library(${PROJECT_NAME}-static STATIC ${${PROJECT_NAME}_sources})
    target_link_libraries(${PROJECT_NAME}-static PUBLIC ${PROJECT_NAME}_compiler_flags)
    target_include_directories(${PROJECT_NAME}-static PUBLIC "${PROJECT_BINARY_DIR}")
    set_target_properties(${PROJECT_NAME}-static PROPERTIES
                          POSITION_INDEPENDENT_CODE ON
                          CXX_VISIBILITY_PRESET hidden)
endif()

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
    set_target_properties(${PROJECT_NAME}_tests PROPERTIES
                          CXX_VISIBILITY_PRESET hidden)

    enable_testing()
    add_test(NAME Tests COMMAND ${PROJECT_NAME}_tests)
endif()
