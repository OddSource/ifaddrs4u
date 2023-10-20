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

package io.oddsource.java.net.ifaddrs4j;

/**
 * We can't put any of this code in the enum classes themselves, because enum constants get initialized
 * before the enum's static initializer is called, so the native methods would result in linkage errors.
 * By putting these helper methods here, we can ensure that the dynamic library is loaded *before* enum
 * constants are initialized.
 *
 * @since 1.0.0
 */
final class EnumConstantsHelper
{
    static
    {
        if (!Loader.LOADED)
        {
            // this will never actually happen, but we need to ensure that Loader initializes
            throw new RuntimeException("Not possible!");
        }
    }

    private EnumConstantsHelper()
    {
    }

    static native int getInterfaceIPAddressFlagConstant(final String name);

    static native int getInterfaceFlagConstant(final String name);
}
