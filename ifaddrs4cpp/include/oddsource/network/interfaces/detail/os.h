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

#ifndef ODDSOURCE_NETWORK_INTERFACES_DETAIL_OS_H
#define ODDSOURCE_NETWORK_INTERFACES_DETAIL_OS_H

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32)
#ifndef ODDSOURCE_IS_WINDOWS
#define ODDSOURCE_IS_WINDOWS 1
#endif
#endif

#ifdef __APPLE__
#ifndef ODDSOURCE_IS_MACOS
#define ODDSOURCE_IS_MACOS 1
#endif
#endif

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
#ifndef ODDSOURCE_IS_BSD
#define ODDSOURCE_IS_BSD 1
#endif
#endif

#include <cstdint>

#endif /* ODDSOURCE_NETWORK_INTERFACES_DETAIL_OS_H */
