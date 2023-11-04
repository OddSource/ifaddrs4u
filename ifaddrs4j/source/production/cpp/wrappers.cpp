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

#include "cache.h"
#include "common.h"
#include "wrappers.h"

namespace OddSource::ifaddrs4j
{
    ::std::string const ArrayList::class_name = "ArrayList"s;

    ArrayList::
    ArrayList(JNIEnv * env, bool)
        : _env(env),
          _class(JCache::c(env, ArrayList::class_name)),
          _add(_class != NULL ? JCache::m(env, _class, ArrayList::class_name, "add(Object)"s) : NULL),
          _wrapped(NULL)
    {
    }

    ArrayList::
    ArrayList(JNIEnv * env)
        : ArrayList(env, true)
    {
        if (this->_add != NULL)
        {
            auto constructor(JCache::m(env, this->_class, ArrayList::class_name, ArrayList::class_name + "()"s));
            if (constructor != NULL)
            {
                this->_wrapped = env->NewObject(this->_class, constructor);
            }
        }
    }

    ArrayList::
    ArrayList(JNIEnv * env, jint capacity)
        : ArrayList(env, true)
    {
        if (this->_add != NULL)
        {
            auto constructor(JCache::m(env, this->_class, ArrayList::class_name, ArrayList::class_name + "(int)"s));
            if (constructor != NULL)
            {
                this->_wrapped = env->NewObject(this->_class, constructor, capacity);
            }
        }
    }

    bool
    ArrayList::
    is_valid()
    {
        return this->_wrapped != NULL;
    }

    bool
    ArrayList::
    add(jobject obj)
    {
        return this->_env->CallBooleanMethod(this->_wrapped, this->_add, obj);
    }

    jobject
    ArrayList::
    unwrap()
    {
        return this->_wrapped;
    }

    ::std::string const Function::class_name = "Function"s;

    Function::
    Function(JNIEnv * env, jobject function)
        : _env(env),
          _wrapped(function),
          _class(JCache::c(env, Function::class_name)),
          _apply(_class != NULL ? JCache::m(env, _class, Function::class_name, "apply(Object)"s) : NULL)
    {
    }

    bool
    Function::
    is_valid()
    {
        return this->_apply != NULL;
    }

    jobject
    Function::
    operator()(jobject obj)
    {
        return this->_env->CallObjectMethod(this->_wrapped, this->_apply, obj);
    }

    ::std::string const BooleanUnboxer::class_name = "Boolean"s;

    BooleanUnboxer::
    BooleanUnboxer(JNIEnv * env)
        : _env(env),
          _class(JCache::c(env, BooleanUnboxer::class_name)),
          _booleanValue(_class != NULL ? JCache::m(env, _class, BooleanUnboxer::class_name, "booleanValue()"s) : NULL)
    {
    }

    bool
    BooleanUnboxer::
    is_valid()
    {
        return this->_booleanValue != NULL;
    }

    bool
    BooleanUnboxer::
    operator()(jobject obj)
    {
        return this->_env->CallBooleanMethod(obj, this->_booleanValue);
    }

    jobject
    Boxers::
    Short(JNIEnv * env, ::std::optional<uint8_t const> const & value)
    {
        if (!value)
        {
            return NULL;
        }

        auto cls(JCache::c(env, "Short"s));
        IF_NULL_RETURN_NULL(cls)
        auto valueOf(JCache::sm(env, cls, "Short"s, "valueOf(short)"s));
        IF_NULL_RETURN_NULL(valueOf)
        return env->CallStaticObjectMethod(cls, valueOf, *value);
    }

    jobject
    Boxers::
    Integer(JNIEnv * env, ::std::optional<uint32_t const> const & value)
    {
        if (!value)
        {
            return NULL;
        }

        auto cls(JCache::c(env, "Integer"s));
        IF_NULL_RETURN_NULL(cls)
        auto valueOf(JCache::sm(env, cls, "Integer"s, "valueOf(int)"s));
        IF_NULL_RETURN_NULL(valueOf)
        return env->CallStaticObjectMethod(cls, valueOf, *value);
    }

    jobject
    Boxers::
    Long(JNIEnv * env, ::std::optional<uint64_t const> const & value)
    {
        if (!value)
        {
            return NULL;
        }

        auto cls(JCache::c(env, "Long"s));
        IF_NULL_RETURN_NULL(cls)
        auto valueOf(JCache::sm(env, cls, "Long"s, "valueOf(long)"s));
        IF_NULL_RETURN_NULL(valueOf)
        return env->CallStaticObjectMethod(cls, valueOf, *value);
    }
}
