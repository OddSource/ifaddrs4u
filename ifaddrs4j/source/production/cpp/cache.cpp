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
#include "macros.h"

#include <iostream>

namespace OddSource::ifaddrs4j
{
    ::std::mutex ClassMethodCache::_singleton_mutex = ::std::mutex();

    ::std::unique_ptr<ClassMethodCache> ClassMethodCache::_singleton = nullptr;

    ::std::unordered_map<::std::string, ClassSearchPath> const ClassMethodCache::_class_name_to_canon = {
        {"Boolean", {"java/lang/Boolean"}},
        {"Short", {"java/lang/Short"}},
        {"Integer", {"java/lang/Integer"}},
        {"Long", {"java/lang/Long"}},
        {"ArrayList", {"java/util/ArrayList"}},
        {"Function", {"java/util/function/Function"}},

        {"IllegalArgumentException", {"java/lang/IllegalArgumentException"}},
        {"IllegalStateException", {"java/lang/IllegalStateException"}},
        {"EnumConstantNotPresentException", {"java/lang/EnumConstantNotPresentException"}},
        {"RuntimeException", {"java/lang/RuntimeException"}},

        {"InetAddressHelper", {"io/oddsource/java/net/ifaddrs4j/InetAddressHelper"}},
        {"MacAddress", {"io/oddsource/java/net/ifaddrs4j/MacAddress"}},
        {"InterfaceIPAddress", {"io/oddsource/java/net/ifaddrs4j/InterfaceIPAddress"}},
        {"Interface", {"io/oddsource/java/net/ifaddrs4j/Interface"}},
    };

    ::std::unordered_map<::std::string, MethodSignature> const ClassMethodCache::_method_name_to_signature = {
        {"Boolean#booleanValue()", {"booleanValue", "()Z"}},
        {"ArrayList#ArrayList()", {"<init>", "()V"}},
        {"ArrayList#ArrayList(int)", {"<init>", "(I)V"}},
        {"ArrayList#add(Object)", {"add", "(Ljava/lang/Object;)Z"}},
        {"Function#apply(Object)", {"apply", "(Ljava/lang/Object;)Ljava/lang/Object;"}},

        {"MacAddress#MacAddress(...)", {"<init>", "(Ljava/lang/String;[B)V"}},
        {"InterfaceIPAddress#InterfaceIPAddress(...)",
         {"<init>",
          "(Ljava/net/InetAddress;ILjava/lang/Short;Ljava/net/InetAddress;Ljava/net/InetAddress;)V"}},
        {"Interface#Interface(...)",
         {"<init>",
          "(ILjava/lang/String;"
#ifdef IS_WINDOWS
          "Ljava/lang/String;"
#endif /* IS_WINDOWS */
          "ILjava/lang/Long;Lio/oddsource/java/net/ifaddrs4j/MacAddress;Ljava/util/List;Ljava/util/List;)V"}},
    };

    ::std::unordered_map<::std::string, MethodSignature> const ClassMethodCache::_static_method_name_to_signature = {
        {"Short#valueOf(short)", {"valueOf", "(S)Ljava/lang/Short;"}},
        {"Integer#valueOf(int)", {"valueOf", "(I)Ljava/lang/Integer;"}},
        {"Long#valueOf(long)", {"valueOf", "(J)Ljava/lang/Long;"}},

        {"InetAddressHelper#getIPv4Address(byte[])", {"getIPv4Address", "([B)Ljava/net/Inet4Address;"}},
        {"InetAddressHelper#getIPv6Address(byte[], Integer)",
         {"getIPv6Address", "([BLjava/lang/Integer;)Ljava/net/Inet6Address;"}},
    };

    void
    ClassMethodCache::
    create_instance(JNIEnv * env)
    {
        ::std::unique_lock<::std::mutex> lock(ClassMethodCache::_singleton_mutex);
        if (ClassMethodCache::_singleton)
        {
            ClassMethodCache::IllegalStateException(
                env,
                "Attempt to load ifaddrs4j dynamic library after it was already loaded");
            return;
        }

        ClassMethodCache::_singleton = ::std::unique_ptr<ClassMethodCache>(new ClassMethodCache);
    }

    void
    ClassMethodCache::
    destroy_instance(JNIEnv * env)
    {
        ::std::unique_lock<::std::mutex> singleton_lock(ClassMethodCache::_singleton_mutex);
        if (!ClassMethodCache::_singleton)
        {
            ClassMethodCache::IllegalStateException(
                env,
                "Attempt to unload ifaddrs4j dynamic library before it was loaded");
            return;
        }

        ClassMethodCache::_singleton.reset();
    }

    bool
    ClassMethodCache::
    ensure_singleton(JNIEnv * env)
    {
        if (!ClassMethodCache::_singleton)
        {
            ClassMethodCache::IllegalStateException(
                env,
                "Attempt to get cached class/method before ifaddrs4j dynamic library was loaded");
            return false;
        }
        return true;
    }

    void
    ClassMethodCache::
    IllegalStateException(JNIEnv * env, char const * message)
    {
        env->ThrowNew(env->FindClass("java/lang/IllegalStateException"), message);
    }

    jclass
    ClassMethodCache::
    c(JNIEnv * env, ::std::string name)
    {
        ::std::unique_lock<::std::mutex> singleton_lock(ClassMethodCache::_singleton_mutex);
        if (!ClassMethodCache::ensure_singleton(env))
        {
            return NULL;
        }
        return ClassMethodCache::_singleton->get_class(env, name);
    }

    jmethodID
    ClassMethodCache::
    m(JNIEnv * env, ::std::string class_name, ::std::string name)
    {
        ::std::unique_lock<::std::mutex> singleton_lock(ClassMethodCache::_singleton_mutex);
        if (!ClassMethodCache::ensure_singleton(env))
        {
            return NULL;
        }
        return ClassMethodCache::_singleton->get_method(env, class_name, name);
    }

    jmethodID
    ClassMethodCache::
    m(JNIEnv * env, jclass cls, ::std::string class_name, ::std::string name)
    {
        ::std::unique_lock<::std::mutex> singleton_lock(ClassMethodCache::_singleton_mutex);
        if (!ClassMethodCache::ensure_singleton(env))
        {
            return NULL;
        }
        return ClassMethodCache::_singleton->get_method(env, cls, class_name, name);
    }

    jmethodID
    ClassMethodCache::
    sm(JNIEnv * env, jclass cls, ::std::string class_name, ::std::string name)
    {
        ::std::unique_lock<::std::mutex> singleton_lock(ClassMethodCache::_singleton_mutex);
        if (!ClassMethodCache::ensure_singleton(env))
        {
            return NULL;
        }
        return ClassMethodCache::_singleton->get_static_method(env, cls, class_name, name);
    }

    ClassMethodCache::
    ClassMethodCache()
        : _class_cache(),
          _method_cache(),
          _static_method_cache()
    {
    }

    ClassMethodCache::
    ~ClassMethodCache()
    {
        ::std::unique_lock<::std::recursive_mutex> lock(this->_mutex);

        // It would seem that we can't delete (and probably there's no point in deleting) all of our global
        // references. On some VMs, this gets called by the C++ runtime when it's shutting down, after the VM
        // has already exited, so we have no way of using the VM or environment to delete the references. On
        // other VMs, this gets called by JNI_OnUnload, so we can use the VM, but then the calls to DeleteGlobalRef
        // result in a segfault, which means the VM cleaned them up *before* unloading the library. So ... we'll
        // just leave the global references and hope everybody else did their jobs properly.

        this->_static_method_cache.clear();
        this->_method_cache.clear();
        this->_class_cache.clear();
    }

    jclass
    ClassMethodCache::
    get_class(JNIEnv * env, ::std::string name)
    {
        return this->get_global_class_ref(env, name);
    }

    jclass
    ClassMethodCache::
    get_global_class_ref(JNIEnv * env, ::std::string name)
    {
        ::std::unique_lock<::std::recursive_mutex> lock(this->_mutex);
        jclass cls(NULL);
        auto found(this->_class_cache.find(name));
        if (found != this->_class_cache.end())
        {
            cls = (jclass) env->NewLocalRef((jobject) found->second);
            // cls may be NULL here if the class the WeakGlobalRef points to was garbage collected
        }
        if (cls == NULL)
        {
            auto canon(ClassMethodCache::_class_name_to_canon.find(name));
            if (canon == ClassMethodCache::_class_name_to_canon.end())
            {
                ClassMethodCache::IllegalStateException(env, (
                    "Attempt to create cache entry for unexpected class name '"s + name +
                    "', for which a search path has not been programmed."s).c_str());
                return NULL;
            }

            cls = env->FindClass(canon->second.path);
            IF_NULL_RETURN_NULL(cls);
            auto weak((jclass) env->NewWeakGlobalRef((jobject) cls));
            IF_NULL_RETURN_NULL(weak);

            this->_class_cache.emplace(name, weak);
        }

        return cls;
    }

    jmethodID
    ClassMethodCache::
    get_method(JNIEnv * env, ::std::string class_name, ::std::string name)
    {
        ::std::unique_lock<::std::recursive_mutex> lock(this->_mutex);
        jclass cls(this->get_global_class_ref(env, class_name));
        IF_NULL_RETURN_NULL(cls);
        return this->get_method(env, cls, class_name, name);
    }

    jmethodID
    ClassMethodCache::
    get_method(JNIEnv * env, jclass cls, ::std::string class_name, ::std::string name)
    {
        return this->get_any_method(
            env, cls, class_name, name,
            ClassMethodCache::_method_name_to_signature, this->_method_cache);
    }

    jmethodID
    ClassMethodCache::
    get_static_method(JNIEnv * env, jclass cls, ::std::string class_name, ::std::string name)
    {
        return this->get_any_method(
            env, cls, class_name, name,
            ClassMethodCache::_static_method_name_to_signature, this->_static_method_cache,
            true);
    }

    jmethodID
    ClassMethodCache::
    get_any_method(
        JNIEnv * env,
        jclass cls,
        ::std::string class_name,
        ::std::string name,
        ::std::unordered_map<::std::string, MethodSignature> const & signature_map,
        ::std::unordered_map<::std::string, jmethodID> & method_cache,
        bool is_static)
    {
        jmethodID method(NULL);
        auto find_name(class_name + "#"s + name);
        auto found(method_cache.find(find_name));
        if (found != method_cache.end())
        {
            method = (jmethodID) env->NewLocalRef((jobject) found->second);
            // method may be NULL here if the method the WeakGlobalRef points to was garbage collected
        }
        if (method == NULL)
        {
            auto signature(signature_map.find(find_name));
            if (signature == signature_map.end())
            {
                ClassMethodCache::IllegalStateException(env, (
                    "Attempt to create cache entry for unexpected "s +
                    (is_static ? "static method name '"s : "method name '"s) +
                    find_name + "', for which a signature has not been programmed."s).c_str());
                return NULL;
            }

            method = is_static ?
                     env->GetStaticMethodID(cls, signature->second.name, signature->second.signature) :
                     env->GetMethodID(cls, signature->second.name, signature->second.signature);
            IF_NULL_RETURN_NULL(method);
            auto weak((jmethodID) env->NewWeakGlobalRef((jobject) method));
            IF_NULL_RETURN_NULL(weak);

            method_cache.emplace(find_name, weak);
        }

        return method;
    }
}

jint JNI_OnLoad(JavaVM * vm, void *)
{
    JNIEnv * env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_10) != JNI_OK)
    {
        return JNI_ERR;
    }

    OddSource::ifaddrs4j::ClassMethodCache::create_instance(env);

    return JNI_VERSION_10;
}

void JNI_OnUnload(JavaVM * vm, void *)
{
    JNIEnv * env;
    vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_10);

    OddSource::ifaddrs4j::ClassMethodCache::destroy_instance(env);
}
