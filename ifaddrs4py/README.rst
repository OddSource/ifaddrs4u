ifaddrs4py - Exploring Network Interfaces
========================================================

ifaddrs4py is a Python 3 + C++ extension for retrieving detailed information about system network interfaces.
Its cross-platform support allows you to examine network interfaces and their addresses without knowing the
details of getifaddrs, GetAdaptersAddresses, or ioctl.

The Python extension is a wrapper around the underlying cross-platform ifaddrs4cpp C++ library. :code:`setup.py`
will automatically locate ifaddrs4cpp sources (:code:`../ifaddrs4cpp` if building from a Git clone or source
archive, :code:`./extern/ifaddrs4cpp` if building from a Python sdist) and build :code:`libifaddrs4cpp-static`
if it has not already been built. The optional :code:`setup.py` argument :code:`--test-cpp` will cause the
ifaddrs4cpp library tests to be compiled and run before compilation can proceed to the Python extension.

For more information, see `ifaddrs4u/README.rst`_ and/or `ifaddrs4cpp/README.rst`_.

.. _ifaddrs4u/README.rst: https://github.com/OddSource/ifaddrs4u
.. _ifaddrs4cpp/README.rst: https://github.com/OddSource/ifaddrs4u/blob/master/ifaddrs4cpp
