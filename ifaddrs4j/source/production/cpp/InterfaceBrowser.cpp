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

#include "generated/io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser.h"

#include <ifaddrs4cpp/Interfaces.h>

#include "cache.h"
#include "Interface.h"
#include "IpAddress.h"
#include "MacAddress.h"
#include "macros.h"
#include "wrappers.h"

namespace
{
    using namespace OddSource::Interfaces;
    using namespace OddSource::ifaddrs4j;

    jfieldID get_field(JNIEnv * env, jobject self, char const * name, char const * signature)
    {
        auto cls(env->GetObjectClass(self));
        IF_NULL_RETURN_NULL(cls)

        return env->GetFieldID(cls, name, signature);
    }

    jfieldID get_pBrowser_field(JNIEnv * env, jobject self)
    {
        return get_field(env, self, "pBrowser", "J");
    }

    jfieldID get_interfaces_field(JNIEnv * env, jobject self)
    {
        return get_field(env, self, "interfaces", "Ljava/util/List;");
    }

    InterfaceBrowser * get_pBrowser(JNIEnv * env, jobject self)
    {
        auto field(get_pBrowser_field(env, self));
        IF_NULL_RETURN_NULL(field)

        jlong pointer(env->GetLongField(self, field));
        if (pointer > -2 && pointer < 2)
        {
            if (env->ExceptionOccurred() == NULL)
            {
                auto IllegalStateException(JCache::c(env, "IllegalStateException"s));
                IF_NULL_RETURN_NULL(IllegalStateException)
                env->ThrowNew(
                    IllegalStateException,
                    "Internal browser pointer is not set, browser has not been initialized.");
            }
            return NULL;
        }

        // This is *waves hands* magic *waves hands* to retrieve the pointer from the Java object
        auto u_pointer(static_cast<uint64_t>(pointer));
        auto browser(reinterpret_cast<InterfaceBrowser *>(u_pointer));
        return browser;
    }

    jobject get_interfaces(JNIEnv * env, jobject self)
    {
        auto field(get_interfaces_field(env, self));
        IF_NULL_RETURN_NULL(field)

        return env->GetObjectField(self, field);
    }

    jobject get_interfaces_list(JNIEnv * env, InterfaceBrowser * browser)
    {
        try
        {
            auto interfaces(browser->get_interfaces());

            ArrayList interfaces_list(env, (jint) interfaces.size());

            for (auto const & iface : interfaces)
            {
                auto jInterface(convert_to_java(env, iface));
                IF_NULL_RETURN_NULL(jInterface)
                if (!interfaces_list.add(jInterface))
                {
                    return NULL;
                }
            }

            return interfaces_list.unwrap();
        }
        CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("RuntimeException"s, return NULL)
    }
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser
 * Method:    init
 * Signature: ()V
 */
void JNICALL
Java_io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_00024NativeBrowser_init(
    JNIEnv * env,
    jobject self)
{
    using namespace OddSource::ifaddrs4j;

    auto field(get_pBrowser_field(env, self));
    IF_NULL_RETURN_VOID(field)

    jlong pointer(env->GetLongField(self, field));
    if (pointer != 0)
    {
        auto IllegalStateException(JCache::c(env, "IllegalStateException"s));
        IF_NULL_STMT(IllegalStateException, return)
        env->ThrowNew(
            IllegalStateException,
            "Duplicate call to init(): Internal browser pointer is already set, browser was already initialized.");
        return;
    }

    try
    {
        // This is *waves hands* magic *waves hands* to store the pointer in the Java object
        auto browser(new OddSource::Interfaces::InterfaceBrowser());
        auto u_pointer(reinterpret_cast<uint64_t>(browser));
        pointer = static_cast<int64_t>(u_pointer);
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("RuntimeException"s, return)

    env->SetLongField(self, field, pointer);
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser
 * Method:    closeInternal
 * Signature: ()V
 */
void JNICALL
Java_io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_00024NativeBrowser_closeInternal(
    JNIEnv * env,
    jobject self)
{
    auto browser(get_pBrowser(env, self));
    IF_NULL_RETURN_VOID(browser)

    delete browser;
}

#define ENSURE_POPULATED(stmt) if (get_interfaces(env, self) == NULL) \
    { \
        if (env->ExceptionOccurred() != NULL) \
        { \
            stmt; \
        } \
        auto interfaces(get_interfaces_list(env, browser)); \
        IF_NULL_STMT(interfaces, stmt) \
        auto field(get_interfaces_field(env, self)); \
        IF_NULL_STMT(field, stmt) \
        env->SetObjectField(self, field, interfaces); \
        if (env->ExceptionOccurred() != NULL) \
        { \
            stmt; \
        } \
    }

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser
 * Method:    getInterface
 * Signature: (Ljava/lang/String;)Lio/oddsource/java/net/ifaddrs4j/Interface;
 */
jobject JNICALL
Java_io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_00024NativeBrowser_getInterface__Ljava_lang_String_2(
    JNIEnv * env,
    jobject self,
    jstring name)
{
    using namespace OddSource::ifaddrs4j;

    auto browser(get_pBrowser(env, self));
    IF_NULL_RETURN_NULL(browser)
    ENSURE_POPULATED(return NULL)

    char const * name_c_str(env->GetStringUTFChars(name, 0));
    IF_NULL_RETURN_NULL(name_c_str)

    ::std::string name_str(name_c_str);
    env->ReleaseStringUTFChars(name, name_c_str);

    try
    {
        return convert_to_java(env, browser->get_interface(name_str));
    }
    catch (::std::invalid_argument const &)
    {
        return NULL;
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("RuntimeException"s, return NULL)
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser
 * Method:    getInterface
 * Signature: (I)Lio/oddsource/java/net/ifaddrs4j/Interface;
 */
jobject JNICALL
Java_io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_00024NativeBrowser_getInterface__I(
    JNIEnv * env,
    jobject self,
    jint index)
{
    using namespace OddSource::ifaddrs4j;

    auto browser(get_pBrowser(env, self));
    IF_NULL_RETURN_NULL(browser)
    ENSURE_POPULATED(return NULL)

    try
    {
        return convert_to_java(env, browser->get_interface(index));
    }
    catch (::std::invalid_argument const &)
    {
        return NULL;
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("RuntimeException"s, return NULL)
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser
 * Method:    ensureInterfacesPopulated
 * Signature: ()V
 */
void JNICALL
Java_io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_00024NativeBrowser_ensureInterfacesPopulated(
    JNIEnv * env,
    jobject self)
{
    auto browser(get_pBrowser(env, self));
    IF_NULL_RETURN_VOID(browser)
    ENSURE_POPULATED(return)
}

/*
 * Class:     io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_NativeBrowser
 * Method:    forEachInterfaceAndPopulate
 * Signature: (Ljava/util/function/Function;)Z
 */
jboolean JNICALL
Java_io_oddsource_java_net_ifaddrs4j_InterfaceBrowser_00024NativeBrowser_forEachInterfaceAndPopulate(
    JNIEnv * env,
    jobject self,
    jobject function)
{
    using namespace OddSource::Interfaces;
    using namespace OddSource::ifaddrs4j;

    auto browser(get_pBrowser(env, self));
    IF_NULL_RETURN_FALSE(browser)

    Function func(env, function);
    IF_INVALID_STMT(func, return false)

    BooleanUnboxer unboxer(env);
    IF_INVALID_STMT(unboxer, return false)

    ArrayList interfaces_list(env);
    IF_INVALID_STMT(interfaces_list, return false)

    bool keep_calling_callable(true);
    bool return_error(false);

    ::std::function<bool(Interface const &)> do_this = [
        env,
        &unboxer,
        &func,
        &interfaces_list,
        &keep_calling_callable,
        &return_error](auto iface)
    {
        auto jInterface(convert_to_java(env, iface));
        if (jInterface == NULL)
        {
            return_error = true;
            return false;
        }
        if (!interfaces_list.add(jInterface))
        {
            return_error = true;
            return false;
        }

        if (keep_calling_callable)
        {
            auto result(func(jInterface));
            if (result == NULL)
            {
                if (env->ExceptionOccurred() == NULL)
                {
                    auto IllegalArgumentException(JCache::c(env, "IllegalArgumentException"));
                    if (IllegalArgumentException != NULL)
                    {
                        env->ThrowNew(
                            IllegalArgumentException,
                            "Function<Interface, Boolean> must return a boolean, not null.");
                    }
                }
                return_error = true;
                return false;
            }
            keep_calling_callable = unboxer(result);
            if (!keep_calling_callable && env->ExceptionOccurred() != NULL)
            {
                return_error = true;
                return false;
            }
        }

        return true;
    };

    try
    {
        browser->for_each_interface(do_this);
    }
    CATCH_STD_EXCEPTION_THROW_EXCEPTION_IF_NOT_THROWN("RuntimeException"s, return false)

    if (!return_error && get_interfaces(env, self) == NULL && env->ExceptionOccurred() == NULL)
    {
        auto field(get_interfaces_field(env, self));
        IF_NULL_RETURN_FALSE(field)
        env->SetObjectField(self, field, interfaces_list.unwrap());
        if (env->ExceptionOccurred() != NULL)
        {
            return false;
        }
    }

    return return_error ? false : keep_calling_callable;
}
