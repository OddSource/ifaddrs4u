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

import static org.junit.Assert.*;

import java.util.Locale;
import java.util.concurrent.atomic.AtomicInteger;

import org.junit.Test;

public class TestInterfaceBrowser
{
    private static final String OS = System.getProperty("os.name").toLowerCase(Locale.US);

    private static final boolean IS_WINDOWS = OS.contains("win");

    private static final boolean IS_MAC = OS.contains("mac") || OS.contains("darwin");

    @Test
    public void test_print_all()
    {
        try(final var browser = new InterfaceBrowser())
        {
            browser.forEachInterface(
                anInterface -> {
                    System.out.println(anInterface.toString());
                    return true;
                }
            );

            final AtomicInteger numInterfaces = new AtomicInteger();
            browser.forEachInterface(ignored -> {
                numInterfaces.getAndIncrement();
                return true;
            });

            assertTrue(numInterfaces.get() > 0);
        }
    }

    @Test
    public void testGetByName()
    {
        try(final var browser = new InterfaceBrowser())
        {
            if(IS_MAC)
            {
                final var lo = browser.getInterface("lo0");
                assertEquals("lo0", lo.getName());
                assertTrue(lo.getIndex() > 0);

                final var en = browser.getInterface("en0");
                assertEquals("en0", en.getName());
                assertTrue(en.getIndex() > 0);

                assertNull(browser.getInterface("fooBar42"));
            }
        }
    }

    @Test
    public void testGetByIndex()
    {
        try(final var browser = new InterfaceBrowser())
        {
            if(IS_MAC)
            {
                final var if1 = browser.getInterface(1);
                assertEquals(1, if1.getIndex());
                assertNotNull(if1.getName());
                assertFalse(if1.getName().isEmpty());

                final var if2 = browser.getInterface(2);
                assertEquals(2, if2.getIndex());
                assertNotNull(if2.getName());
                assertFalse(if2.getName().isEmpty());

                assertNull(browser.getInterface(Integer.MAX_VALUE - 2));
            }
        }
    }

    @Test
    public void testGetInterfaces()
    {
        int interfacesFound = 0;
        boolean loopbackFound = false;
        boolean nonLoopbackFound = false;

        try(final var browser = new InterfaceBrowser())
        {
            final var interfaces = browser.getInterfaces();
            while(interfaces.hasMoreElements())
            {
                final var anInterface = interfaces.nextElement();
                interfacesFound++;
                if(anInterface.isFlagEnabled(InterfaceFlag.IsLoopback))
                {
                    loopbackFound = true;
                }
                else
                {
                    nonLoopbackFound = true;
                }
            }
        }
        assertTrue(
            "At least two interfaces should have been found, but " + interfacesFound + " found instead.",
            interfacesFound > 1
        );
        assertTrue("At least one loopback interface should have been found.", loopbackFound);
        assertTrue("At least one non-loopback interface should have been found.", nonLoopbackFound);
    }
}
