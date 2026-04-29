ifaddrs4cpp - Exploring Network Interfaces in C++
=================================================

ifaddrs4cpp is a C++ library (static or shared) for retrieving detailed information about system network interfaces.
Its cross-platform support allows you to examine network interfaces and their addresses without knowing the
details of :code:`getifaddrs`, :code:`GetAdaptersAddresses`, or :code:`ioctl`, etc.

Other Languages
***************

Native extensions wrapping this C++ library are available for `Java`_, `Python`_, and (TBD) `Perl`_.

System Requirements
*******************

When used as a static library, there are no runtime system requirements. When used as a shared library, a
C++17 Standard Runtime library must be available on the library path applicable to your platform.

Build Requirements
******************

- Clang 13 or higher, GCC 8 or higher, or Visual Studio 2019 or higher (on Windows), or any other comparable
  compiler capable of compiling for the C++17 standard.
- GNU Make, unless you're on Windows using Visual Studio 2019
- CMake 3.22 or newer installed and available on the path

Building
********

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

    $ cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS:BOOL=ON -S . -B ./cmake-build-test
    $ cmake --build ./cmake-build-test --config Debug -j 14
    $ ctest --test-dir ./cmake-build-test --build-config Debug --verbose --test-action Test --output-on-failure

To do the same, but enable Address Sanitizer on macOS and Linux (and Leak Sanitizer on Linux) for analysis
purposes::

    $ cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS:BOOL=ON -DENABLE_ADDRESS_SANITIZER:BOOL=ON -S . -B ./cmake-build-test-with-asan
    $ cmake --build ./cmake-build-test-with-asan --config Debug -j 14
    -- on macOS
    $ ASAN_OPTIONS=detect_stack_use_after_return=1:verify_asan_link_order=0 ./cmake-build-test-with-asan/ifaddrs4cpp_tests
    -- on Linux
    $ ASAN_OPTIONS=detect_stack_use_after_return=1:detect_leaks=1:verify_asan_link_order=0 ./cmake-build-test-with-asan/ifaddrs4cpp_tests

Boost Support
*************

ifaddrs4cpp supports converting IP address objects to Boost IP address objects. To enable this, define
:code:`ODDSOURCE_INCLUDE_BOOST` before including any ifaddrs4cpp headers and/or at build time with
:code:`-DODDSOURCE_INCLUDE_BOOST`.

If you need your Boost namespace to be something other than :code:`boost`,
define :code:`ODDSOURCE_BOOST_NAMESPACE_ROOT=my_boost` to indicate an alternate namespace. If you need the include root
to be something other than :code:`boost`, define :code:`ODDSOURCE_BOOST_HEADER_ROOT=my_boost` to indicate an alternate
root.

To run tests with Boost support, include :code:`-DBOOST_ROOT=/path/to/boot` or
:code:`-DBoost_DIR=/path/to/boost` in the cmake commands above. This will have the side effect of
automatically defining :code:`ODDSOURCE_INCLUDE_BOOST` for that cmake build.

Usage
*****

This section is to-be-completed.

.. _Java: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4j/README.rst
.. _Python: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4py/README.rst
.. _Perl: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4pl/README.rst
