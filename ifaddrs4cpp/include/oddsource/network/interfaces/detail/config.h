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
#include <oddsource/network/interfaces/detail/version.h>

#include "export.h"

#if defined(ODDSOURCE_IS_WINDOWS) && defined(ODDSOURCE_BUILDING_LIBRARY)
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#endif /* defined(ODDSOURCE_IS_WINDOWS) && defined(ODDSOURCE_BUILDING_LIBRARY) */

#define ODDSOURCE_HEADER_BRACKETS(prefix, suffix) <prefix/suffix>
#define ODDSOURCE_HEADER_EXPAND(prefix, suffix) ODDSOURCE_HEADER_BRACKETS(prefix, suffix)
#define ODDSOURCE_HEADER(prefix, suffix) ODDSOURCE_HEADER_EXPAND(prefix, suffix)

#ifdef ODDSOURCE_INCLUDE_BOOST

#  ifndef ODDSOURCE_BOOST_NAMESPACE_ROOT
#    define ODDSOURCE_BOOST_NAMESPACE_ROOT boost
#  endif /* ODDSOURCE_BOOST_NAMESPACE_ROOT */

#  ifdef ODDSOURCE_BOOST_HEADER_ROOT
#    define ODDSOURCE_BOOST_HEADER(suffix) ODDSOURCE_HEADER(ODDSOURCE_BOOST_INCLUDE_ROOT, suffix)
#  else /* ODDSOURCE_BOOST_HEADER_ROOT */
#    define ODDSOURCE_BOOST_HEADER(suffix) ODDSOURCE_HEADER(boost, suffix)
#  endif /* !ODDSOURCE_BOOST_HEADER_ROOT */

#endif /* ODDSOURCE_INCLUDE_BOOST */

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_CONFIG_H */
