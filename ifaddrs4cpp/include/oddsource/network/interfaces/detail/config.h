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

#ifndef ODDSOURCE_NETWORK_INTERFACES_DETAIL_CONFIG_H
#define ODDSOURCE_NETWORK_INTERFACES_DETAIL_CONFIG_H

#include "os.h"
#include "version.h"

#include "export.h"

#if defined(ODDSOURCE_IS_WINDOWS) && defined(ODDSOURCE_BUILDING_LIBRARY)
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#endif /* defined(ODDSOURCE_IS_WINDOWS) && defined(ODDSOURCE_BUILDING_LIBRARY) */

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_CONFIG_H */
