#pragma once

#include "config.h"

#include <ifaddrs4cpp/MacAddress.h>

namespace OddSource::ifaddrs4py
{
    void init_mac_address_sample(PyObject *);

    PyObject * convert_to_python(OddSource::Interfaces::MacAddress const &);
}

PyObject *
extern_get_mac_address_data_from_repr(PyObject *, PyObject * const *, Py_ssize_t);

PyObject *
extern_get_mac_address_repr_from_data(PyObject *, PyObject * const *, Py_ssize_t);
