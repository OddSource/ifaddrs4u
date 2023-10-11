cmake_minimum_required(VERSION 3.26)

project(ifaddrs4cpp
        VERSION 0.1.0
        DESCRIPTION "A C++17 library for cross-platform access to network interfaces and their addresses"
        HOMEPAGE_URL "https://github.com/OddSource/ifaddrs4u"
        LANGUAGES CXX)
set(PROJECT_VERSION_SUFFIX "alpha.0")

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
set(${PROJECT_NAME}_sources
    config.h
    Interface.h
    Interface.hpp
    Interface.cpp
    Interfaces.h
    Interfaces.hpp
    Interfaces.cpp
    IpAddress.h
    IpAddress.hpp
    IpAddress.cpp
    MacAddress.h
    MacAddress.cpp
    MacAddress.hpp
    os.h)

configure_file(version.h.in version.h)

add_library(${PROJECT_NAME}_compiler_flags INTERFACE)
target_compile_features(${PROJECT_NAME}_compiler_flags INTERFACE cxx_std_17)
target_compile_options(${PROJECT_NAME}_compiler_flags INTERFACE
                       "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wdeprecated;-Wextra;-Wpedantic;-Wshadow;-Wunused;-Werror>>"
                       "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>")
set_target_properties(${PROJECT_NAME}_compiler_flags PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

add_library(${PROJECT_NAME}-shared SHARED ${${PROJECT_NAME}_sources})
target_link_libraries(${PROJECT_NAME}-shared PUBLIC ${PROJECT_NAME}_compiler_flags)
target_include_directories(${PROJECT_NAME}-shared PUBLIC "${PROJECT_BINARY_DIR}")

add_library(${PROJECT_NAME}-static STATIC ${${PROJECT_NAME}_sources})
target_link_libraries(${PROJECT_NAME}-static PUBLIC ${PROJECT_NAME}_compiler_flags)
target_include_directories(${PROJECT_NAME}-static PUBLIC "${PROJECT_BINARY_DIR}")

option(RUN_TESTS "If enabled, tests will be compiled and executed." False)

add_executable(${PROJECT_NAME}_tests
               tests/main.h
               tests/main.hpp
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

if (${RUN_TESTS})
    enable_testing()
    add_test(NAME Tests COMMAND ${PROJECT_NAME}_tests)
endif()
