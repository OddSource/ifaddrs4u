ifaddrs4u - Exploring Network Interfaces, Universally
=====================================================

ifaddrs4u is a universal, platform-independent tool for obtaining information about network interfaces (adapters)
and their hardware, IPv4, and IPv6 addresses and properties and configuration flags without knowing the details
of :code:`getifaddrs`, :code:`GetAdaptersAddresses`, or :code:`ioctl`, etc.

The name uses a `common Java library idiom`_ of the library purpose followed by "4" (an English homonym of "for")
followed by the language (normally "j", but "net" is also popular for C#), e.g. log4j, neo4j, log4net, etc. In
this case, the "u" means "universal," but you can also think of it as "you." "ifaddrs" refers to the fact that
this library primarily wraps the Unix/Linux function :code:`getifaddrs` and its struct :code:`ifaddrs` (on
Windows, it wraps :code:`GetAdaptersAddresses` / :code:`PIP_ADAPTER_ADDRESSES`). `See here`_ (`or here`_) for
more information on these platform-specific libraries.

.. contents:: Contents
    :depth: 3

Libraries
*********

This project contains the following language-specific libraries (and more may be added in the future):

:code:`ifaddrs4cpp`
-------------------

This is the core library upon which all the other language-specific libraries depend. It's based on C++17
and can be used directly, if desired, as either a static library (:code:`.a` / :code:`.lib`) or a shared/dynamic
library (:code:`.so` / :code:`.dylib` / :code:`.dll`). If used as a shared library, you must also load a
C++17 or newer standard library. For specific information about building and using this library, see
`ifaddrs4cpp/README.rst`_.

:code:`ifaddrs4j`
-----------------

This is a Java library that makes use of a combination of standard JDK classes and JNI wrappers around
:code:`ifaddrs4cpp`. It is designed to be API-similar to :code:`ifaddrs4cpp`, but there are some necessary
differences to meet Java rules, requirements, and best practices. For specific information about building and
using this library, see `ifaddrs4j/README.rst`_.

:code:`ifaddrs4pl`
------------------

This is an incomplete work in progress. More information soon.

:code:`ifaddrs4py`
------------------

This is a Pyton library that majes use of a combination of standard Python classes and Python native wrappers
around :code:`ifaddrs4cpp`. It is designed to be API-similar to :code:`ifaddrs4cpp`, but there are some necessary
differences to meet Python rules, requirements, and best practices. For specific information about building
and using this library, see `ifaddrs4py/README.rst`_.

License
*******

This entire project and all of it language-specific libraries are governed by the `Apache License, Version 2.0`_.


.. _common Java library idiom: https://stackoverflow.com/questions/1826014/what-does-4j-mean
.. _See here: https://linux.die.net/man/3/getifaddrs
.. _or here: https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersaddresses
.. _ifaddrs4cpp/README.rst: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4cpp/README.rst
.. _ifaddrs4j/README.rst: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4j/README.rst
.. _ifaddrs4py/README.rst: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4py/README.rst
.. _Apache License, Version 2.0: https://www.apache.org/licenses/LICENSE-2.0
