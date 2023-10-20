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

#include "generated/io_oddsource_java_net_ifaddrs4j_VersionInfo.h"

#include <ifaddrs4cpp/VersionInfo.h>

#include <string>

jstring JNICALL Java_io_oddsource_java_net_ifaddrs4j_VersionInfo_getExtensionVersion(JNIEnv * env, jclass)
{
    return env->NewStringUTF(OddSource::Interfaces::VersionInfo::version().c_str());
}

jshort JNICALL Java_io_oddsource_java_net_ifaddrs4j_VersionInfo_getExtensionMajor(JNIEnv *, jclass)
{
    return IFADDRS4CPP_VERSION_MAJOR;
}

jshort JNICALL Java_io_oddsource_java_net_ifaddrs4j_VersionInfo_getExtensionMinor(JNIEnv *, jclass)
{
    return IFADDRS4CPP_VERSION_MINOR;
}

jshort JNICALL Java_io_oddsource_java_net_ifaddrs4j_VersionInfo_getExtensionPatch(JNIEnv *, jclass)
{
    return IFADDRS4CPP_VERSION_PATCH;
}

jstring JNICALL Java_io_oddsource_java_net_ifaddrs4j_VersionInfo_getExtensionSuffix(JNIEnv * env, jclass)
{
    auto suffix = OddSource::Interfaces::VersionInfo::suffix();
    return suffix ? env->NewStringUTF((*suffix).c_str()) : NULL;
}
