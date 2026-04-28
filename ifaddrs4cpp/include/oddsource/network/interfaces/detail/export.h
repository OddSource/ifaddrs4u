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

#ifndef ODDSOURCE_NETWORK_INTERFACES_DETAIL_EXPORT_H
#define ODDSOURCE_NETWORK_INTERFACES_DETAIL_EXPORT_H

#ifdef IFADDRS4CPP_INLINE_SOURCE

#define OddSource_Export
#define OddSource_Extern
#define OddSource_Inline inline

#else /* IFADDRS4CPP_INLINE_SOURCE */

#ifdef ODDSOURCE_IS_WINDOWS
#ifdef ODDSOURCE_BUILDING_LIBRARY
#pragma message("COMPILATION DEBUG: Defining OddSource_Export as __declspec(dllexport)")
#define OddSource_Export __declspec(dllexport)
#else /* ODDSOURCE_BUILDING_LIBRARY */
#pragma message("COMPILATION DEBUG: Defining OddSource_Export as __declspec(dllimport)")
#define OddSource_Export __declspec(dllimport)
#endif /* !ODDSOURCE_BUILDING_LIBRARY */
#else /* ODDSOURCE_IS_WINDOWS */
#define OddSource_Export __attribute((visibility("default")))
#endif /* !ODDSOURCE_IS_WINDOWS */

#define OddSource_Extern extern
#define OddSource_Inline

#endif /* !IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_EXPORT_H */
