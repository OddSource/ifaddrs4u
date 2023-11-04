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

import java.util.Locale;
import java.util.concurrent.atomic.AtomicInteger;

import org.junit.Test;

import io.oddsource.java.net.ifaddrs4j.InterfaceBrowser;
import io.oddsource.java.net.ifaddrs4j.InterfaceFlag;

public class TestInterfaceBrowser
{
    private static final String OS = System.getProperty("os.name").toLowerCase(Locale.US);

    private static final boolean IS_WINDOWS = OS.contains("win");

    private static final boolean IS_MAC = OS.contains("mac") || OS.contains("darwin");

    public TestInterfaceBrowser()
    {
    }

    @Test
    public void testForEachThenIterate()
    {
        try(var browser = new InterfaceBrowser())
        {
            final var numInterfaces1 = new AtomicInteger();
            final var result = browser.forEachInterface(anInterface -> {
                numInterfaces1.getAndIncrement();
                System.err.println(anInterface.toString());
                return true;
            });

            int numInterfaces2 = 0;
            for(var anInterface : browser)
            {
                numInterfaces2++;
                assertTrue(anInterface.getIndex() > 0);
                assertNotNull(anInterface.getName());
            }

            assertTrue(result);
            assertTrue(numInterfaces1.get() > 0);
            assertEquals(numInterfaces1.get(), numInterfaces2);
        }
    }

    @Test
    public void testIterateThenForEach()
    {
        try(var browser = new InterfaceBrowser())
        {
            int numInterfaces1 = 0;
            for(var anInterface : browser)
            {
                numInterfaces1++;
                assertTrue(anInterface.getIndex() > 0);
                assertNotNull(anInterface.getName());
            }

            final var numInterfaces2 = new AtomicInteger();
            final var result = browser.forEachInterface(ignored -> {
                numInterfaces2.getAndIncrement();
                return true;
            });

            assertTrue(result);
            assertTrue(numInterfaces1 > 0);
            assertEquals(numInterfaces1, numInterfaces2.get());
        }
    }

    @Test
    public void testIterateThenIterate()
    {
        try(var browser = new InterfaceBrowser())
        {
            int numInterfaces1 = 0;
            for(var anInterface : browser)
            {
                numInterfaces1++;
                assertTrue(anInterface.getIndex() > 0);
                assertNotNull(anInterface.getName());
            }

            int numInterfaces2 = 0;
            for(var anInterface : browser)
            {
                numInterfaces2++;
                assertTrue(anInterface.getIndex() > 0);
                assertNotNull(anInterface.getName());
            }

            assertTrue(numInterfaces1 > 0);
            assertEquals(numInterfaces1, numInterfaces2);
        }
    }

    @Test
    public void testForEachWithReturningFalse()
    {
        try(var browser = new InterfaceBrowser())
        {
            final var numInterfaces1 = new AtomicInteger();
            final var result1 = browser.forEachInterface(ignored -> {
                numInterfaces1.getAndIncrement();
                return false;
            });

            final var numInterfaces2 = new AtomicInteger();
            final var result2 = browser.forEachInterface(ignored -> numInterfaces2.getAndIncrement() == 0);

            assertFalse(result1);
            assertFalse(result2);
            assertEquals(1, numInterfaces1.get());
            assertEquals(2, numInterfaces2.get());
        }
    }

    @Test
    public void testGetByName()
    {
        try(var browser = new InterfaceBrowser())
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
        try(var browser = new InterfaceBrowser())
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

        try(var browser = new InterfaceBrowser())
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
