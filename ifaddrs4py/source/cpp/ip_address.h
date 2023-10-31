/*
 * Copyright © 2010-2023 OddSource Code (license@oddsource.io)
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

#include <ifaddrs4cpp/IpAddress.h>

namespace OddSource::ifaddrs4py
{
    void init_ip_address_samples(PyObject *);

    PyObject * convert_to_python(OddSource::Interfaces::IPv4Address const &);

    PyObject * convert_to_python(OddSource::Interfaces::IPv6Address const &);
}
