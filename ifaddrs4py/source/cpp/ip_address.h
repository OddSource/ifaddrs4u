#pragma once

#include "config.h"

#include <ifaddrs4cpp/IpAddress.h>

namespace OddSource::ifaddrs4py
{
    PyObject * convert_to_python(OddSource::Interfaces::IPv4Address const &);

    PyObject * convert_to_python(OddSource::Interfaces::IPv6Address const &);

    void init_test_ip_address_constants(PyObject *);
}
