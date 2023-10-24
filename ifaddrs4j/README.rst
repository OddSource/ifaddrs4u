ifaddrs4j - Exploring Network Interfaces in Java
================================================

ifaddrs4j is a Java + C++ extension for retrieving detailed information about system network interfaces.
Its cross-platform support allows you to examine network interfaces and their addresses without knowing the
details of :code:`getifaddrs`, :code:`GetAdaptersAddresses`, or :code:`ioctl`, etc.

.. contents:: Contents
    :depth: 3

System Requirements
*******************

At runtime, the only requirement is Java 11 or newer. See below for additional build requirements.

Build Requirements
******************

- You must have Maven installed.
- In order to compile the C++ extension in this Java library, you must have the JNI Development Headers
  installed. The way to do this varies depending on how you installed Java (system package manager,
  Homebrew, Cygwin, downloadable installer, official Java Docker container, etc.). See your Java
  distribution documentation for more information.
- There are additional requirements for compiling :code:`ifaddrs4cpp`. See `ifaddrs4cpp/README.rst`_.

Installing and Building
***********************

This section is to-be-completed.

Usage
*****

This section is to-be-completed.

.. _ifaddrs4cpp/README.rst: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4cpp/README.rst
