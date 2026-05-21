ifaddrs4cpp - Exploring Network Interfaces in C++
=================================================

.. contents:: Table of Contents
   :local:
   :depth: 2

ifaddrs4cpp is a C++ library (static, shared, or header-only) for retrieving detailed information about system network
interfaces. Its cross-platform support allows you to examine network interfaces and their addresses without knowing the
details of :code:`getifaddrs`, :code:`GetAdaptersAddresses`, or :code:`ioctl`, etc.

Other Languages
***************

Native extensions wrapping this C++ library are available for `Java`_, `Python`_, and (maybe, TBD) `Perl`_.

System Requirements
*******************

When used as a static library, there are no runtime system requirements. When used as a shared library, a
C++17/20/23 Standard Runtime library must be available on the library path applicable to your platform.

Building ifaddrs4cpp
******************++

If not using ifaddrs4cpp as a header-only library, you'll need the following to build it:

- Clang 17 or higher, GCC 9 or higher, or Visual Studio 2022 or higher (on Windows), or any other comparable
  compiler capable of compiling for the C++17, C++20, and/or C++23 standards.
- GNU Make, unless you're on Windows using Visual Studio 2022+
- CMake 3.22 or newer installed and available on the path

Building
--------

The simplest build, which will output both a static library and a shared/dynamic library without debug symbols::

    $ cmake -DCMAKE_BUILD_TYPE=Release -S . -B ./cmake-build-release
    $ cmake --build ./cmake-build-release --config Release -j 14

To build just a static library, without debug symbols::

    $ cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_STATIC_ONLY:BOOL=ON -S . -B ./cmake-build-release
    $ cmake --build ./cmake-build-release --config Release -j 14

To build just a shared/dynamic library, without debug symbols::

    $ cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_DYNAMIC_ONLY:BOOL=ON -S . -B ./cmake-build-release
    $ cmake --build ./cmake-build-release --config Release -j 14

If you want debug symbols, change :code:`Release` to :code:`Debug` and :code:`release` to :code:`debug` in both
commands.

To compile the library and tests and run the tests::

    $ ./test

Or more manually::

    $ cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS:BOOL=ON -S . -B ./cmake-build-test
    $ cmake --build ./cmake-build-test --config Debug -j 14
    $ ctest --test-dir ./cmake-build-test --build-config Debug --verbose --test-action Test --output-on-failure

To do the same, but enable Address Sanitizer on macOS and Linux (and Leak Sanitizer on Linux) for analysis
purposes::

    $ ./test-asan

Or more manually::

    $ cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS:BOOL=ON -DENABLE_ADDRESS_SANITIZER:BOOL=ON -S . -B ./cmake-build-test-with-asan
    $ cmake --build ./cmake-build-test-with-asan --config Debug -j 14
    -- on macOS
    $ ASAN_OPTIONS=detect_stack_use_after_return=1:verify_asan_link_order=0 MallocNanoZone=0 ./cmake-build-test-with-asan/ifaddrs4cpp_tests
    -- on Linux
    $ ASAN_OPTIONS=detect_stack_use_after_return=1:detect_leaks=1:verify_asan_link_order=0 ./cmake-build-test-with-asan/ifaddrs4cpp_tests

Changing C++ Standard Version
-----------------------------

The default C++ version automatically used is C++17. However, you can change the version by passing the argument
:code:`-DCPP_VERSION=20` or :code:`-DCPP_VERSION=23` to the CMake configuration commands when building. If you are using
ifaddrs4cpp as a header-only library, all that matters is the way you compile your application/library, and you don't
need to take any additional steps.

Boost Support
*************

ifaddrs4cpp supports converting IP address objects to and from Boost.ASIO IP address objects. To enable this, define
:code:`IFADDRS4CPP_INCLUDE_BOOST` before including any ifaddrs4cpp headers in your application/library, and/or when
your application/library is built with :code:`-DIFADDRS4CPP_INCLUDE_BOOST`.

If you need your Boost root namespace to be something other than :code:`boost` (i.e. you have transformed Boost
namespaces using `the Boost Copy Tool (bcp)`_), define :code:`IFADDRS4CPP_BOOST_NAMESPACE_ROOT=my_boost` to indicate an
alternate namespace (:code:`my_boost` in this example). If doing this, you may also need to change the root directory
of the Boost include path to be something other than :code:`boost`. If you need to do this (for any reason) define
:code:`IFADDRS4CPP_BOOST_HEADER_ROOT=my_boost` to indicate an alternate root.

.. _the Boost Copy Tool (bcp): https://www.boost.org/doc/libs/1_91_0/tools/bcp/doc/html/index.html

All Boost support is inline/header-only (there are no exported symbols), so ifaddrs4cpp does not need to be
built/rebuilt with Boost support enabled. All that matters is how your application/library is built. However, if Boost
is installed in a standard location on your system, the CMake build will automatically find it, add it to the *test*
compilation include path, and define :code:`IFADDRS4CPP_INCLUDE_BOOST` in the *test* compilation macros so that
additional tests are executed to verify the Boost.ASIO IP address integration. If you do not want it to do this,
include :code:`-DSKIP_BOOST:BOOL=ON` in the CMake configuration commands above. If you do want to test Boost code but
Boost is in a non-standard location that CMake cannot automatically find, include :code:`-DBOOST_ROOT=/path/to/boot`
in the CMake configuration commands above or set the environment variable :code:`BOOST_ROOT=/path/to/boot` before
invoking the CMake configuration commands above. The directory pointed to by :code:`BOOST_ROOT` must be a "built" Boost
root containing a :code:`BoostConfig.cmake` file.

Usage
*****

Macro Definitions
-----------------

- :code:`IFADDRS4CPP_INLINE_SOURCE`: Define this macro when building your application/library if you want it to use
  ifaddrs4cpp as a header-only library. You will not need to build/install ifaddrs4cpp binaries or link against the
  library in order to use it in this case.
- :code:`IFADDRS4CPP_STATIC_LINKAGE`: If you are using ifaddrs4cpp as a static library on Windows, define this macro
  when building your application/library to prevent :code:`__declspec(dllimport)` attributes' being added to symbols in
  included headers. If you are using it as a static library on any other operating system, this is not required;
  however, to support multiple platforms at once, it is safe to define this macro on any platform (it is ignored on
  non-Windows platforms).
- :code:`IFADDRS4CPP_INCLUDE_BOOST`: Define this macro when building your application/library if you want to make use
  of support for Boost.ASIO IP addresses (see above).
- :code:`IFADDRS4CPP_BOOST_HEADER_ROOT`: Define this macro both when building this library (if applicable) and when
  building your application/library if you're using Boost.ASIO IP addresses and the root directory in the Boost include
  path is something other than :code:`boost` (see above).
- :code:`IFADDRS4CPP_BOOST_NAMESPACE_ROOT`: Define this macro both when building this library (if applicable) and when
  building your application/library if you're using Boost.ASIO IP addresses and the root Boost namespace is something
  other than :code:`boost` (see above).

Including Headers
-----------------

The headers in :code:`oddsource/network/interfaces/detail` and :code:`oddsource/network/interfaces/impl` are not meant
to be included directly. They are included as necessary at build time or inline based on the macros you do or don't
define when building your application (see above). To use ifaddrs4cpp, include one or more of the following headers in
:code:`oddsource/network/interfaces`:

- :code:`Interfaces.hpp`: provides the class :code:`InterfaceBrowser`, which is the primary API for accessing the
  system's network interfaces, as well as exception :code:`InterfaceBrowserSystemError`. In most cases, this is the
  only header you will need to include, as it includes all the other headers directly or indirectly.
- :code:`MacAddress.hpp`: provides the class :code:`MacAddress` and exception :code:`InvalidMacAddress`, as well as
  streaming operators and :code:`toString` overloads.
- :code:`IPAddress.hpp`: provides the class :code:`IPAddress`, exception :code:`InvalidIPAddress`, and enum classes
  :code:`IPAddressVersion`, :code:`MulticastScope`, :code:`MulticastV6Flag`, as well as streaming operators and
  :code:`toString` overloads.
- :code:`Interface.hpp`: provides the template class :code:`InterfaceIPAddress` and its instantiations
  :code:`InterfaceIPv4Address` and :code:`InterfaceIPv6Address`, the class :code:`Interface`, and the enum classes
  :code:`InterfaceIPAddressFlag` and :code:`InterfaceFlag`, as well as streaming operators and :code:`toString`
  overloads.
- :code:`VersionInfo.hpp`: provides the static class :code:`VersionInfo` providing programmatic access to the
  ifaddrs4cpp library version information.
- :code:`WSAHelper.hpp`: On Windows only, this header provides an RAII-patterned helper class
  :code:`WinSockStartupCleanupHelper` that performs the necessary Windows Socket API startup operations on construction
  and cleanup operations on destruction. On non-Windows platforms, the header still exists, but including it is a no-op.

API Documentation
-----------------

TODO

Examples
--------

TODO

.. _Java: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4j/README.rst
.. _Python: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4py/README.rst
.. _Perl: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4pl/README.rst
