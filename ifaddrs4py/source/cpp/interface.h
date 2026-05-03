/*
 * Copyright © 2010-2026 OddSource Code (license@oddsource.io)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "config.h"

#include <oddsource/network/interfaces/Interface.hpp>

namespace OddSource::ifaddrs4py
{
    void
    initInterfaceEnums(
        PyObject * module );

    template< class IPAddressT >
    PyObject *
    convertToPython(
        OddSource::Interfaces::InterfaceIPAddress< IPAddressT > const & interfaceAddress );

    PyObject *
    convertToPython(
        OddSource::Interfaces::Interface const & rInterface );
}

PyObject *
extern_get_sample_interface_ipv4_address(
    PyObject * moduleSelf,
    PyObject * args );

PyObject *
extern_get_sample_interface_ipv6_address(
    PyObject * moduleSelf,
    PyObject * args );

PyObject *
extern_get_sample_interface_scoped_ipv6_address(
    PyObject * moduleSelf,
    PyObject * args );

PyObject *
extern_get_sample_interface(
    PyObject * moduleSelf,
    PyObject * args );
