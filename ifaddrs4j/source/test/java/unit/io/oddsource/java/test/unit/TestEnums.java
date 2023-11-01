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

package io.oddsource.java.test.unit;

import static org.junit.Assert.*;

import org.junit.Test;

import io.oddsource.java.net.ifaddrs4j.InterfaceFlag;
import io.oddsource.java.net.ifaddrs4j.InterfaceIPAddressFlag;

public class TestEnums
{
    public TestEnums()
    {
    }

    @Test
    public void testInterfaceIPAddressFlags()
    {
        for (var e : InterfaceIPAddressFlag.values())
        {
            System.err.println("InterfaceIPAddressFlag." + e.name() + " (" + e.display + ") = " + e.flag);
            assertTrue("InterfaceIPAddressFlag." + e.name() + " is not correct", e.flag > 0);
        }
    }

    @Test
    public void testInterfaceFlags()
    {
        for (var e : InterfaceFlag.values())
        {
            System.err.println("InterfaceFlag." + e.name() + " (" + e.display + ") = " + e.flag);
            assertTrue("InterfaceFlag." + e.name() + " is not correct", e.flag > 0);
        }

        assertNotEquals(InterfaceFlag.IsUp.flag, InterfaceFlag.UNSUPPORTED_FLAG);
        assertNotEquals(InterfaceFlag.IsRunning.flag, InterfaceFlag.UNSUPPORTED_FLAG);
        assertNotEquals(InterfaceFlag.IsLoopback.flag, InterfaceFlag.UNSUPPORTED_FLAG);
    }
}
