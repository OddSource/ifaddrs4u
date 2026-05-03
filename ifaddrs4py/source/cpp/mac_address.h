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

#include <oddsource/network/interfaces/MacAddress.hpp>

namespace OddSource::ifaddrs4py
{
    void
    initMacAddressSamples(
        PyObject * module );

    PyObject *
    convertToPython(
        OddSource::Interfaces::MacAddress const & address );
}

PyObject *
extern_get_mac_address_data_from_repr(
    PyObject * moduleSelf,
    PyObject * const * args,
    Py_ssize_t nargs );

PyObject *
extern_get_mac_address_repr_from_data(
    PyObject * moduleSelf,
    PyObject * const * args,
    Py_ssize_t nargs );
