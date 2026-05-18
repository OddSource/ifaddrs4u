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

#if defined( ODDSOURCE_BUILDING_SHARED_LIBRARY )
#  pragma message("COMPILATION DEBUG: Defining OddSource_Export as __declspec(dllexport) in DLL")
#  define OddSource_Export __declspec(dllexport)
#elif defined( ODDSOURCE_BUILDING_STATIC_LIBRARY ) /* ODDSOURCE_BUILDING_SHARED_LIBRARY */
#  pragma message("COMPILATION DEBUG: Defining OddSource_Export as [nothing] in static library")
#  define OddSource_Export
#elif defined( IFADDRS4CPP_STATIC_LINKAGE ) /* ODDSOURCE_BUILDING_STATIC_LIBRARY */
#  define OddSource_Export
#else /* IFADDRS4CPP_STATIC_LINKAGE */
#  define OddSource_Export __declspec(dllimport)
#endif /* !anything */

#else /* ODDSOURCE_IS_WINDOWS */

#define OddSource_Export __attribute((visibility("default")))

#endif /* !ODDSOURCE_IS_WINDOWS */

#define OddSource_Extern extern
#define OddSource_Inline

#endif /* !IFADDRS4CPP_INLINE_SOURCE */

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_EXPORT_H */
