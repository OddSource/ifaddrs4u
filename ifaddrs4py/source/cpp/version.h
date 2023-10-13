#pragma once

#define PY_SSIZE_T_CLEAN  /* Make "s#" use Py_ssize_t rather than int. */
#include <Python.h>

namespace OddSource::ifaddrs4py
{
    void init_version_constants(PyObject * module);
}
