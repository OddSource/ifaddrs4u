cmake_minimum_required(VERSION 3.26)

project(ifaddrs4j
        VERSION 0.1.0
        DESCRIPTION "A JNI C++17 extension for ifaddrs4j"
        HOMEPAGE_URL "https://github.com/OddSource/ifaddrs4u"
        LANGUAGES CXX)

set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(gcc_like_ids ARMClang AppleClang Clang GNU LCC)
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")

set(CMAKE_VERBOSE_MAKEFILE TRUE)
if(CMAKE_CXX_COMPILER_ID IN_LIST gcc_like_ids)
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
endif()
set(CMAKE_CXX_EXTENSIONS OFF)

set(IS_WINDOWS FALSE)
set(IS_MACOS FALSE)
set(COPY_COMMAND "cp")
set(STATIC_LIBRARY_EXTENSION "a")
set(DYNAMIC_LIBRARY_EXTENSION "so")
set(JAVA_PLATFORM_INCLUDE "linux")
if(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Windows")
    set(IS_WINDOWS TRUE)
    set(COPY_COMMAND "copy")
    set(STATIC_LIBRARY_EXTENSION "lib")
    set(DYNAMIC_LIBRARY_EXTENSION "dll")
    set(JAVA_PLATFORM_INCLUDE "win32") # TODO
elseif(${CMAKE_HOST_SYSTEM_NAME} MATCHES "Darwin")
    set(IS_MACOS TRUE)
    set(DYNAMIC_LIBRARY_EXTENSION "dylib")
    set(JAVA_PLATFORM_INCLUDE "darwin")
endif()

set(SOURCES_DIRECTORY source/production/cpp)

set(${PROJECT_NAME}_sources
    ${SOURCES_DIRECTORY}/cache.cpp
    ${SOURCES_DIRECTORY}/Interface.cpp
    ${SOURCES_DIRECTORY}/InterfaceBrowser.cpp
    ${SOURCES_DIRECTORY}/IpAddress.cpp
    ${SOURCES_DIRECTORY}/MacAddress.cpp
    ${SOURCES_DIRECTORY}/Samples.cpp
    ${SOURCES_DIRECTORY}/VersionInfo.cpp)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}_compiler_flags INTERFACE
                       "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wdeprecated;-Wextra;-Wpedantic;-Wshadow;-Wunused;-Werror>>"
                       "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>")
set_target_properties(${PROJECT_NAME}_compiler_flags PROPERTIES
                      CXX_STANDARD_REQUIRED ON
                      CXX_EXTENSIONS OFF)

add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_sources})
target_link_libraries(${PROJECT_NAME} PUBLIC
                      ${PROJECT_NAME}_compiler_flags
                      "${CMAKE_SOURCE_DIR}/../ifaddrs4cpp/cmake-external-build/libifaddrs4cpp-static-d.${STATIC_LIBRARY_EXTENSION}"
                      "$ENV{JAVA_HOME}/lib/libjava.${DYNAMIC_LIBRARY_EXTENSION}")
target_include_directories(${PROJECT_NAME} PUBLIC
                           "${CMAKE_SOURCE_DIR}/../ifaddrs4cpp/cmake-external-build/include"
                           "$ENV{JAVA_HOME}/include"
                           "$ENV{JAVA_HOME}/include/${JAVA_PLATFORM_INCLUDE}")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND
                   ${COPY_COMMAND} "${CMAKE_BINARY_DIR}/libifaddrs4j.${DYNAMIC_LIBRARY_EXTENSION}" "${CMAKE_SOURCE_DIR}/target/classes/")
