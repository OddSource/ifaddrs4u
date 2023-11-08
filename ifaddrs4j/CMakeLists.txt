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

project(ifaddrs4j
        VERSION 0.1.0
        DESCRIPTION "A JNI C++17 extension for ifaddrs4j"
        HOMEPAGE_URL "https://github.com/OddSource/ifaddrs4u"
        LANGUAGES CXX)

set(CLASSIFIER "" CACHE STRING "Classifier, including leading hyphen, to append to copied dynamic library name")
option(ENABLE_ADDRESS_SANITIZER
       "If enabled, Address Sanitizer (ASAN) will be enabled on macOS and Linux, and also Leak Sanitizer (LSAN) on Linux"
       False)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(gcc_like_ids ARMClang AppleClang Clang GNU LCC)
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(CMAKE_VERBOSE_MAKEFILE TRUE)
if(CMAKE_CXX_COMPILER_ID IN_LIST gcc_like_ids)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "The macOS target" FORCE)

set(STATIC_LIBRARY_EXTENSION "a")
set(DYNAMIC_LIBRARY_EXTENSION "so")
set(IFADDRS_LIBRARY_PREFIX "lib")
set(LIB_JAVA "lib/libjava")
set(JAVA_PLATFORM_INCLUDE "linux")
if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    set(STATIC_LIBRARY_EXTENSION "lib")
    set(DYNAMIC_LIBRARY_EXTENSION "dll")
    set(IFADDRS_LIBRARY_PREFIX "${CMAKE_BUILD_TYPE}/")
    set(LIB_JAVA "bin/java")
    set(JAVA_PLATFORM_INCLUDE "win32")
elseif(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    set(DYNAMIC_LIBRARY_EXTENSION "dylib")
    set(JAVA_PLATFORM_INCLUDE "darwin")
endif()

set(IFADDRS_LIBRARY_SUFFIX "")
if(${CMAKE_BUILD_TYPE} MATCHES "Debug")
    set(IFADDRS_LIBRARY_SUFFIX "-d")
endif()

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
            add_compile_options(-fsanitize-address-use-odr-indicator -shared-libsan)
            add_link_options(-fsanitize-address-use-odr-indicator -shared-libsan)
        endif()
    endif()
endif()

set(SOURCES_DIRECTORY source/production/cpp)

set(${PROJECT_NAME}_sources
    ${SOURCES_DIRECTORY}/cache.cpp
    ${SOURCES_DIRECTORY}/Interface.cpp
    ${SOURCES_DIRECTORY}/InterfaceBrowser.cpp
    ${SOURCES_DIRECTORY}/IpAddress.cpp
    ${SOURCES_DIRECTORY}/MacAddress.cpp
    ${SOURCES_DIRECTORY}/Samples.cpp
    ${SOURCES_DIRECTORY}/VersionInfo.cpp
    ${SOURCES_DIRECTORY}/wrappers.cpp)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}_compiler_flags INTERFACE
                       "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wdeprecated;-Wextra;-Wpedantic;-Wshadow;-Wunused;-Werror>>"
                       "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3;-wd4275;-wd4251;-wd4455>>")
target_compile_definitions(${PROJECT_NAME}_compiler_flags INTERFACE ODDSOURCE_BUILDING_LIBRARY=1)
set_target_properties(${PROJECT_NAME}_compiler_flags PROPERTIES
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_sources})
set_target_properties(${PROJECT_NAME} PROPERTIES
                      CXX_VISIBILITY_PRESET hidden)
if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
# Apparently, we do not have to link to java.dll on Windows like we link to java.so/dylib on oter platforms.
    target_link_libraries(${PROJECT_NAME} PUBLIC
                          ${PROJECT_NAME}_compiler_flags
                          "${CMAKE_SOURCE_DIR}/../ifaddrs4cpp/cmake-external-build/${IFADDRS_LIBRARY_PREFIX}ifaddrs4cpp-static${IFADDRS_LIBRARY_SUFFIX}.${STATIC_LIBRARY_EXTENSION}")
else()
    target_link_libraries(${PROJECT_NAME} PUBLIC
                          ${PROJECT_NAME}_compiler_flags
                          "${CMAKE_SOURCE_DIR}/../ifaddrs4cpp/cmake-external-build/${IFADDRS_LIBRARY_PREFIX}ifaddrs4cpp-static${IFADDRS_LIBRARY_SUFFIX}.${STATIC_LIBRARY_EXTENSION}"
                          "$ENV{JAVA_HOME}/${LIB_JAVA}.${DYNAMIC_LIBRARY_EXTENSION}")
endif()
target_include_directories(${PROJECT_NAME} PUBLIC
                           "${CMAKE_SOURCE_DIR}/../ifaddrs4cpp/cmake-external-build/include"
                           "$ENV{JAVA_HOME}/include"
                           "$ENV{JAVA_HOME}/include/${JAVA_PLATFORM_INCLUDE}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:ifaddrs4j>" "${CMAKE_SOURCE_DIR}/target/classes/libifaddrs4j${CLASSIFIER}.${DYNAMIC_LIBRARY_EXTENSION}")
