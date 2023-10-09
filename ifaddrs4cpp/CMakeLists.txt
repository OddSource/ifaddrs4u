cmake_minimum_required(VERSION 3.26)

project(ifaddrs4cpp
        VERSION 0.1.0
        DESCRIPTION "A C++17 library for cross-platform access to network interfaces and their addresses"
        HOMEPAGE_URL "https://github.com/OddSource/ifaddrs4u"
        LANGUAGES CXX)
set(PROJECT_VERSION_SUFFIX "alpha.0")
set(ifaddrs4cpp_VERSION_SUFFIX "${PROJECT_VERSION_SUFFIX}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_VERBOSE_MAKEFILE TRUE)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wdeprecated -Wall -Wextra -Wpedantic -Werror")

configure_file(version.h.in version.h)

add_library(ifaddrs4cpp Interfaces.cpp
        IpAddress.h
        config.h
        IpAddress.cpp
        IpAddress.hpp
        Interface.h
        MacAddress.h
        MacAddress.cpp
        Interface.cpp
        Interface.hpp
        os.h
        MacAddress.hpp)
