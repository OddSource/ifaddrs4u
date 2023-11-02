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

#include <ifaddrs4cpp/os.h>

#define IF_NULL_RETURN(thing, ret) if (thing == NULL) \
    { \
        return ret; \
    }

#define IF_NULL_RETURN_INT(thing) IF_NULL_RETURN(thing, -1)
#define IF_NULL_RETURN_NULL(thing) IF_NULL_RETURN(thing, NULL)
#define IF_NULL_RETURN_FALSE(thing) IF_NULL_RETURN(thing, false)
#define IF_NULL_RETURN_VOID(thing) IF_NULL_RETURN(thing, )
#define IF_NULL_RETURN_ERR(thing) IF_NULL_RETURN(thing, JNI_ERR)

#define CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN(err, ret) catch (::std::exception const & e) \
    { \
        if (env->ExceptionOccurred() == NULL) \
        { \
            env->ThrowNew(err, e.what()); \
        } \
        ret; \
    }
