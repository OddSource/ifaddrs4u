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

#pragma  once

#ifndef ODDSOURCE_NETWORK_INTERFACES_DETAIL_WINSOCK_INCLUDES_H
#define ODDSOURCE_NETWORK_INTERFACES_DETAIL_WINSOCK_INCLUDES_H

#ifdef ODDSOURCE_IS_WINDOWS

// Windows and Windows socket headers have to be included in a particular order, or else
// all kinds of errors are encountered. Additionally, many (all?) of these header files
// do not have double-inclusion guards. Both of these facts are incredible stupid, but
// we have to account for them. So all Windows includes will be in this file, in the
// correct order, with double-inclusion guards around them.
#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <ip2string.h>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "Ntdll.lib")

#endif /* ODDSOURCE_IS_WINDOWS */

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_WINSOCK_INCLUDES_H */
