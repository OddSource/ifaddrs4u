#pragma once

#include "config.h"

#include <ifaddrs4cpp/IpAddress.h>

namespace OddSource::ifaddrs4py
{
    void init_ip_address_samples(PyObject *);

    PyObject * convert_to_python(OddSource::Interfaces::IPv4Address const &);

    PyObject * convert_to_python(OddSource::Interfaces::IPv6Address const &);
}
