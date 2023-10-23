#pragma once

#include "config.h"

#include <ifaddrs4cpp/Interface.h>

namespace OddSource::ifaddrs4py
{
    void init_interface_enums(PyObject *);

    template<class IPAddressT>
    PyObject * convert_to_python(OddSource::Interfaces::InterfaceIPAddress<IPAddressT> const &);

    PyObject * convert_to_python(OddSource::Interfaces::Interface const &);
}

PyObject *
extern_get_sample_interface_ipv4_address(PyObject *, PyObject *);

PyObject *
extern_get_sample_interface_ipv6_address(PyObject *, PyObject *);

PyObject *
extern_get_sample_interface_scoped_ipv6_address(PyObject *, PyObject *);

PyObject *
extern_get_sample_interface(PyObject *, PyObject *);
