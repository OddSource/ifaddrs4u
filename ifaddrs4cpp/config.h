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

#pragma  once

#include "os.h"
#include "version.h"

#ifdef IS_WINDOWS
#ifdef ODDSOURCE_BUILDING_LIBRARY
#define OddSource_Export __declspec(dllexport)
#else /* ODDSOURCE_BUILDING_LIBRARY */
#define OddSource_Export __declspec(dllimport)
#endif /* !ODDSOURCE_BUILDING_LIBRARY */
#else /* IS_WINDOWS */
#define OddSource_Export __attribute((visibility("default")))
#endif /* IS_WINDOWS */

#ifdef IS_WINDOWS
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#endif /* IS_WINDOWS */
