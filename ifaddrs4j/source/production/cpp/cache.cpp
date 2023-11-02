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

namespace OddSource::ifaddrs4j
{
    jclass JNICache::InetAddressHelper = NULL;
    jmethodID JNICache::InetAddressHelper_getIPv4Address = NULL;
    jmethodID JNICache::InetAddressHelper_getIPv6Address = NULL;

    jclass JNICache::MacAddress = NULL;
    jmethodID JNICache::MacAddress__init_ = NULL;

    jclass JNICache::InterfaceIPAddress = NULL;
    jmethodID JNICache::InterfaceIPAddress__init_ = NULL;

    jclass JNICache::Interface = NULL;
    jmethodID JNICache::Interface__init_ = NULL;

    jclass JNICache::Boolean = NULL;
    jmethodID JNICache::Boolean_booleanValue = NULL;

    jclass JNICache::Short = NULL;
    jmethodID JNICache::Short_valueOf = NULL;

    jclass JNICache::Integer = NULL;
    jmethodID JNICache::Integer_valueOf = NULL;

    jclass JNICache::Long = NULL;
    jmethodID JNICache::Long_valueOf = NULL;

    jclass JNICache::ArrayList = NULL;
    jmethodID JNICache::ArrayList__init_ = NULL;
    jmethodID JNICache::ArrayList__init__int = NULL;
    jmethodID JNICache::ArrayList_add = NULL;

    jclass JNICache::Function = NULL;
    jmethodID JNICache::Function_apply = NULL;

    jclass JNICache::IllegalArgumentException = NULL;
    jclass JNICache::IllegalStateException = NULL;
    jclass JNICache::EnumConstantNotPresentException = NULL;
    jclass JNICache::RuntimeException = NULL;
}

jint JNI_OnLoad(JavaVM * vm, void *)
{
    JNIEnv * env;
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_10) != JNI_OK)
    {
        return JNI_ERR;
    }

    using namespace OddSource::ifaddrs4j;

    jclass cls;
    jmethodID method;

    // Boolean methods
    cls = env->FindClass("java/lang/Boolean");
    JNICache::Boolean = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::Boolean)

    method = env->GetMethodID(cls, "booleanValue", "()Z");
    JNICache::Boolean_booleanValue = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::Boolean_booleanValue)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // Short methods
    cls = env->FindClass("java/lang/Short");
    JNICache::Short = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::Short)

    method = env->GetStaticMethodID(cls, "valueOf", "(S)Ljava/lang/Short;");
    JNICache::Short_valueOf = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::Short_valueOf)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // Integer methods
    cls = env->FindClass("java/lang/Integer");
    JNICache::Integer = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::Integer)

    method = env->GetStaticMethodID(cls, "valueOf", "(I)Ljava/lang/Integer;");
    JNICache::Integer_valueOf = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::Integer_valueOf)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // Long methods
    cls = env->FindClass("java/lang/Long");
    JNICache::Long = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::Long)

    method = env->GetStaticMethodID(cls, "valueOf", "(J)Ljava/lang/Long;");
    JNICache::Long_valueOf = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::Long_valueOf)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // ArrayList methods
    cls = env->FindClass("java/util/ArrayList");
    JNICache::ArrayList = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::ArrayList)

    method = env->GetMethodID(cls, "<init>", "()V");
    JNICache::ArrayList__init_ = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::ArrayList__init_)
    env->DeleteLocalRef((jobject) method);

    method = env->GetMethodID(cls, "<init>", "(I)V");
    JNICache::ArrayList__init__int = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::ArrayList__init__int)
    env->DeleteLocalRef((jobject) method);

    method = env->GetMethodID(cls, "add", "(Ljava/lang/Object;)Z");
    JNICache::ArrayList_add = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::ArrayList_add)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // Function apply
    cls = env->FindClass("java/util/function/Function");
    JNICache::Function = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::Function)

    method = env->GetMethodID(cls, "apply", "(Ljava/lang/Object;)Ljava/lang/Object;");
    JNICache::Function_apply = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_ERR(JNICache::Function_apply)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // Exception classes
    cls = env->FindClass("java/lang/IllegalArgumentException");
    JNICache::IllegalArgumentException = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::IllegalArgumentException)
    env->DeleteLocalRef(cls);

    cls = env->FindClass("java/lang/IllegalStateException");
    JNICache::IllegalStateException = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::IllegalStateException)
    env->DeleteLocalRef(cls);

    cls = env->FindClass("java/lang/EnumConstantNotPresentException");
    JNICache::EnumConstantNotPresentException = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::EnumConstantNotPresentException)
    env->DeleteLocalRef(cls);

    cls = env->FindClass("java/lang/RuntimeException");
    JNICache::RuntimeException = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_ERR(JNICache::RuntimeException)
    env->DeleteLocalRef(cls);

    return JNI_VERSION_10;
}

bool
OddSource::ifaddrs4j::JNICache::
ensure_our_classes_loaded(JNIEnv * env)
{
    if (JNICache::Interface__init_ != NULL)
    {
        return true;
    }

    jclass cls;
    jmethodID method;

    // InetAddressHelper methods
    cls = env->FindClass("io/oddsource/java/net/ifaddrs4j/InetAddressHelper");
    JNICache::InetAddressHelper = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_FALSE(JNICache::InetAddressHelper)

    method = env->GetStaticMethodID(cls, "getIPv4Address", "([B)Ljava/net/Inet4Address;");
    JNICache::InetAddressHelper_getIPv4Address = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_FALSE(JNICache::InetAddressHelper_getIPv4Address)
    env->DeleteLocalRef((jobject) method);

    method = env->GetStaticMethodID(cls, "getIPv6Address", "([BLjava/lang/Integer;)Ljava/net/Inet6Address;");
    JNICache::InetAddressHelper_getIPv6Address = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_FALSE(JNICache::InetAddressHelper_getIPv6Address)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // MacAddress constructor
    cls = env->FindClass("io/oddsource/java/net/ifaddrs4j/MacAddress");
    JNICache::MacAddress = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_FALSE(JNICache::MacAddress)

    // MacAddress(String, byte[])
    method = env->GetMethodID(cls, "<init>", "(Ljava/lang/String;[B)V");
    JNICache::MacAddress__init_ = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_FALSE(JNICache::MacAddress__init_)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // InterfaceIPAddress constructor
    cls = env->FindClass("io/oddsource/java/net/ifaddrs4j/InterfaceIPAddress");
    JNICache::InterfaceIPAddress = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_FALSE(JNICache::InterfaceIPAddress)

    // InterfaceIPAddress(T, int, Short, T, T)
    method = env->GetMethodID(
         cls,
         "<init>",
         "(Ljava/net/InetAddress;ILjava/lang/Short;Ljava/net/InetAddress;Ljava/net/InetAddress;)V");
    JNICache::InterfaceIPAddress__init_ = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_FALSE(JNICache::InterfaceIPAddress__init_)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    // Interface constructor
    cls = env->FindClass("io/oddsource/java/net/ifaddrs4j/Interface");
    JNICache::Interface = (jclass) env->NewGlobalRef(cls);
    IF_NULL_RETURN_FALSE(JNICache::Interface)

    // Interface(int, String[, String], Long, MacAddress, List<InterfaceIPAddress<Inet4Address>>, List<InterfaceIPAddress<Inet6Address>>)
    // (ILjava/lang/String;ILjava/lang/Long;Lio/oddsource/java/net/ifaddrs4j/MacAddress;Ljava/util/List;Ljava/util/List;)V
    method = env->GetMethodID(cls, "<init>", (
        "(ILjava/lang/String;"s +
#ifdef IS_WINDOWS
        "Ljava/lang/String;"s +
#endif /* IS_WINDOWS */
        "ILjava/lang/Long;Lio/oddsource/java/net/ifaddrs4j/MacAddress;Ljava/util/List;Ljava/util/List;)V"s).c_str());
    JNICache::Interface__init_ = (jmethodID) env->NewGlobalRef((jobject) method);
    IF_NULL_RETURN_FALSE(JNICache::Interface__init_)
    env->DeleteLocalRef((jobject) method);

    env->DeleteLocalRef(cls);

    return true;
}

void JNI_OnUnload(JavaVM * vm, void *)
{
    JNIEnv * env;
    vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_10);

    using namespace OddSource::ifaddrs4j;

    env->DeleteGlobalRef(JNICache::InetAddressHelper);
    env->DeleteGlobalRef((jobject) JNICache::InetAddressHelper_getIPv4Address);
    env->DeleteGlobalRef((jobject) JNICache::InetAddressHelper_getIPv6Address);

    env->DeleteGlobalRef(JNICache::MacAddress);
    env->DeleteGlobalRef((jobject) JNICache::MacAddress__init_);

    env->DeleteGlobalRef(JNICache::InterfaceIPAddress);
    env->DeleteGlobalRef((jobject) JNICache::InterfaceIPAddress__init_);

    env->DeleteGlobalRef(JNICache::Interface);
    env->DeleteGlobalRef((jobject) JNICache::Interface__init_);

    env->DeleteGlobalRef(JNICache::Boolean);
    env->DeleteGlobalRef((jobject) JNICache::Boolean_booleanValue);

    env->DeleteGlobalRef(JNICache::Short);
    env->DeleteGlobalRef((jobject) JNICache::Short_valueOf);

    env->DeleteGlobalRef(JNICache::Integer);
    env->DeleteGlobalRef((jobject) JNICache::Integer_valueOf);

    env->DeleteGlobalRef(JNICache::Long);
    env->DeleteGlobalRef((jobject) JNICache::Long_valueOf);

    env->DeleteGlobalRef(JNICache::ArrayList);
    env->DeleteGlobalRef((jobject) JNICache::ArrayList__init_);
    env->DeleteGlobalRef((jobject) JNICache::ArrayList__init__int);
    env->DeleteGlobalRef((jobject) JNICache::ArrayList_add);

    env->DeleteGlobalRef(JNICache::Function);
    env->DeleteGlobalRef((jobject) JNICache::Function_apply);

    env->DeleteGlobalRef(JNICache::IllegalArgumentException);
    env->DeleteGlobalRef(JNICache::IllegalStateException);
    env->DeleteGlobalRef(JNICache::EnumConstantNotPresentException);
    env->DeleteGlobalRef(JNICache::RuntimeException);
}
