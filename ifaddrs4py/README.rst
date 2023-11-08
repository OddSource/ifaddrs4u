ifaddrs4py - Exploring Network Interfaces in Python
===================================================

ifaddrs4py is a Python + C++ extension for retrieving detailed information about system network interfaces.
Its cross-platform support allows you to examine network interfaces and their addresses without knowing the
details of :code:`getifaddrs`, :code:`GetAdaptersAddresses`, or :code:`ioctl`, etc.

The Python extension is a wrapper around the underlying cross-platform ifaddrs4cpp C++ library. :code:`setup.py`
will automatically locate ifaddrs4cpp sources (:code:`../ifaddrs4cpp` if building from a Git clone or source
archive, :code:`./extern/ifaddrs4cpp` if building from a Python sdist) and build :code:`libifaddrs4cpp-static`
if it has not already been built.

.. contents:: Contents
    :depth: 3

System Requirements
*******************

At runtime, the only requirement is Python 3.8 or newer. See below for additional build requirements.

Build Requirements
******************

- You must have Pip installed.
- In order to compile the C++ extension in this Python library, you must have the Python Development Headers
  installed. The way to do this varies depending on how you installed Python (system package manager,
  Homebrew, Cygwin, downloadable installer, official Python Docker container, pyenv, etc.). See your Python
  distribution documentation for more information.
- In order to build the binary distribution (bdist wheel) or source distribution (sdist), you must have the
  `Build`_ project installed: :code:`pip install build`.
- There are additional requirements for compiling :code:`ifaddrs4cpp`. See `ifaddrs4cpp/README.rst`_.

Installing and Building
***********************

- From PyPi::

    $ pip install ifaddrs4py

  This will install the binary installation if one is available. Otherwise, it will download, compile,
  and install the source destination. See `Build Requirements`_.

- From source, compile and install only (see `Build Requirements`_)::

    $ git clone https://github.com/OddSource/ifaddrs4u.git
    $ cd ifaddrs4u/ifaddrs4py
    $ pip install --verbose .
    - or, to run CPP tests before installing -
    $ pip install -C--build-option=--cpp-test --verbose .
    - as another option, you can clean all C++ Compile output before installing -
    $ pip install -C--build-option=--clean --verbose .
    - to run tests and other checks -
    $ pip install --verbose .[test]
    $ pytest --verbose -s
    $ mypy .
    $ flake8 .
    - to build and test with Address Sanitizer to check for memory misuse and leaks -
    $ pip install -C--build-option=--clean -C--build-option=--cpp-test -C--build-option=--cpp-debug -C--build-option=--cpp-asan --verbose .[test]
    $ ./python_asan -m pytest --verbose -s

  Do not use :code:`python setup.py install`, as this is a deprecated convention that will cause deprecation
  warnings. See `Why you shouldn't invoke setup.py directly`_ for more information.

- From source, create and upload bdist wheel (see `Build Requirements`_)::

    $ git clone https://github.com/OddSource/ifaddrs4u.git
    $ cd ifaddrs4u/ifaddrs4py
    $ python -m build --wheel
    - or, to run CPP tests before building wheel -
    $ python -m build --wheel -C--build-option=--cpp-test

- From source, create and upload sdist (see `Build Requirements`_)::

    $ git clone https://github.com/OddSource/ifaddrs4u.git
    $ cd ifaddrs4u/ifaddrs4py
    $ python -m build --sdist

For more information, see `ifaddrs4u/README.rst`_ and/or `ifaddrs4cpp/README.rst`_.

Usage
*****

This section is to-be-completed.


.. _Why you shouldn't invoke setup.py directly: https://blog.ganssle.io/articles/2021/10/setup-py-deprecated.html#summary
.. _ifaddrs4u/README.rst: https://github.com/OddSource/ifaddrs4u
.. _ifaddrs4cpp/README.rst: https://github.com/OddSource/ifaddrs4u/blob/main/ifaddrs4cpp/README.rst
.. _Build: https://pypa-build.readthedocs.io/en/stable/
