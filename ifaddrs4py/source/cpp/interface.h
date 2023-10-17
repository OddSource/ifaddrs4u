#pragma once

#include "config.h"

#include <ifaddrs4cpp/Interface.h>
#include <ifaddrs4cpp/MacAddress.h>

namespace OddSource::ifaddrs4py
{
    void init_interface_enums(PyObject *);

    PyObject * convert_to_python(OddSource::Interfaces::MacAddress const &);

    template<class IPAddressT>
    PyObject * convert_to_python(OddSource::Interfaces::InterfaceIPAddress<IPAddressT> const &);

    PyObject * convert_to_python(OddSource::Interfaces::Interface const &);
}
