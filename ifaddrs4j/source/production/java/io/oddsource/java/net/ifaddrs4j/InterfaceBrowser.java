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

import java.lang.ref.Cleaner;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import java.util.function.Function;
import java.util.stream.Stream;

/**
 * A tool for browsing network interfaces/adapters configured on this system. To ensure memory resources are cleaned
 * up properly, be sure to {@link #close()} the browser when you are done with it, such as with a try-with-resources
 * block. Example uses are detailed below.<br>
 * <br>
 * Iterate over the browser to inspect all interfaces:
 * <pre>{@code
 * try(var browser = new InterfaceBrowser())
 * {
 *     ...
 *     for(var anInterface : browser)
 *     {
 *         System.out.println(anInterface.toString());
 *     }
 *     ...
 * }
 * }</pre>
 * For-each all interfaces with a lambda, stopping iteration when the lambda returns {@code false}:
 * <pre>{@code
 * try(var browser = new InterfaceBrowser())
 * {
 *     ...
 *     var notFound = browser.forEachInterface(iface -> {
 *         if(... some test against iface ...)
 *         {
 *             ... // use the iface
 *             return false; // to stop iteration
 *         }
 *         return true; // to continue iteration
 *     });
 *     if(notFound)
 *     {
 *         ... // react to this situation
 *     }
 * }
 * }</pre>
 * Other actions you can take, all of which should be surrounded with try-with-resources:
 * <pre>{@code
 * // get an interface if you know its system name (on Windows, you can use its
 * // UUID string or friendly name; either will match)
 * var iface = browser.getInterface("eth0");
 *
 * // get an interface if you know its system index
 * var iface = browser.getInterface(1);
 *
 * // get an enumeration of all interfaces
 * var interfaces = browser.getInterfaces();
 *
 * // stream over the interfaces, finding the first non-loopback, status-up interface
 * // with at least one non-link-local IPv6 address
 * var iface = browser.interfaces().
 *                     filter(i -> i.isFlagEnabled(InterfaceFlag.IsUp) &&
 *                                 !i.isFlagEnabled(InterfaceFlag.IsLoopback)).
 *                     findFirst(i -> i.ipv6Addresses().findFirst(
 *                         a -> !a.getAddress().isLinkLocalAddress()
 *                     ) != null);
 * }</pre>
 * @implNote
 *     This browser's data does not become "exhausted," though it could become stale. Upon first post-construction
 *     use of {@link #interfaces()}, {@link #iterator()}, {@link #forEachInterface(Function)},
 *     {@link #getInterfaces()}, {@link #getInterface(String)}, or {@link #getInterface(int)}, the browser holds
 *     a snapshot-in-time of the system's network interface configuration, which may immediately become stale. These
 *     six accessor methods can be used any number of times, in any order, in any combination, and by multiple
 *     threads if necessary.
 *
 * @since 1.0.0
 */
public final class InterfaceBrowser implements AutoCloseable, Iterable<Interface>
{
    static
    {
        if(!Loader.LOADED)
        {
            // this will never actually happen, but we need to ensure that Loader initializes
            throw new RuntimeException("Not possible!");
        }
    }

    private static final Cleaner cleaner = Cleaner.create();

    private final NativeBrowser internal;

    private final Cleaner.Cleanable cleanable;

    /**
     * Construct an interface browser.
     */
    public InterfaceBrowser()
    {
        this.internal = new NativeBrowser();
        this.cleanable = InterfaceBrowser.cleaner.register(this, new BrowserCleaner(this.internal));
    }

    @Override
    public void close()
    {
        this.cleanable.clean();
    }

    private synchronized void ready()
    {
        if(this.internal == null || this.internal.isClosed())
        {
            throw new IllegalStateException("This interface browser has been closed or is in an unusable state.");
        }
    }

    /**
     * Get the interface with the given name, or {@code null} if no such interface exists. On Windows,
     * an attempt will be made to match the given name against the friendly name, the UUID, and the
     * UUID with curly braces around it.<br>
     *
     * @implNote
     *     The complexity of this method is {@code O(n)} on the first call to any of the six accessor
     *     methods on this instance and then {@code O(1)} for all subsequent calls.
     *
     * @param name The interface name or, optionally on Windows (only), the UUID or <code>{UUID}</code>
     * @return the matched interface or {@code null}.
     */
    public Interface getInterface(final String name)
    {
        this.ready();
        return this.internal.getInterface(name);
    }

    /**
     * Get the interface with the given index, or {@code null} if no such interface exists.<br>
     *
     * @implNote
     *     The complexity of this method is {@code O(n)} on the first call to any of the six accessor
     *     methods on this instance and then {@code O(1)} for all subsequent calls.
     *
     * @param index The interface index
     * @return the matched interface or {@code null}.
     */
    public Interface getInterface(final int index)
    {
        this.ready();
        return this.internal.getInterface(index);
    }

    /**
     * Get an iterable enumeration of all interfaces configured on the system.<br>
     * <br>
     * Note that there is no guaranteed order in which interfaces will be enumerated, and that the
     * order can change from one call to the next (though that would not be typical). If you need
     * particular order, consider Streaming {@link #interfaces()} and sorting the stream.
     *
     * @return an enumeration of interfaces.
     */
    public Enumeration<Interface> getInterfaces()
    {
        this.ready();
        return new InterfaceEnumeration(this.internal.getInterfaces().listIterator());
    }

    /**
     * Get an unmodifiable iterator of all interfaces configured on the system, which will throw
     * {@link UnsupportedOperationException} upon an attempt to call {@link Iterator#remove()}.<br>
     * <br>
     * Note that there is no guaranteed order in which interfaces will be iterated, and that the
     * order can change from one call to the next (though that would not be typical). If you need
     * a particular order, consider Streaming {@link #interfaces()} and sorting the stream.
     *
     * @return an iterator of interfaces.
     */
    @Override
    public Iterator<Interface> iterator()
    {
        this.ready();
        return new InterfaceIterator(this.internal.getInterfaces().listIterator());
    }

    /**
     * Get a filterable stream of all interfaces configured on the system.<br>
     *
     * @implNote
     *     That there is no guaranteed order in which interfaces will be streamed unless you
     *     {@link Stream#sorted(java.util.Comparator) sort} the stream. The only guarantee is that subsequent streams
     *     will always be in the same order, and that streaming, {@link #iterator() iterating},
     *     {@link #getInterfaces() enumerating}, and {@link #forEachInterface(Function) for-each-ing} will occur in
     *     the same order.
     *
     * @return a stream of interfaces.
     */
    public Stream<Interface> interfaces()
    {
        this.ready();
        return this.internal.getInterfaces().stream();
    }

    /**
     * For each interface configured on the system, apply the supplied function, stopping when
     * the function returns {@code false}.<br>
     * <br>
     * Note that there is no guaranteed order in which interfaces will be iterated, and that the
     * order can change from one call to the next (though that would not be typical). If you need
     * a particular order, consider Streaming {@link #interfaces()} and sorting the stream.
     *
     * @param doThis The function to apply to each interface
     * @return {@code true} if the function was applied to all interfaces, {@code false} if the function
     *         ever returned false to stop application. As an edge case, this may return {@code false}
     *         if the function was applied to all interfaces but the function returned {@code false} on
     *         the very last interface.
     */
    public boolean forEachInterface(final Function<Interface, Boolean> doThis)
    {
        this.ready();
        return this.internal.forEachInterface(doThis);
    }

    @Override
    public String toString()
    {
        final var builder = new StringBuilder("<io.oddsource.java.net.ifaddrs4j.InterfaceBrowser: ");
        if (this.internal == null)
        {
            builder.append("uninitialized/unusable");
        }
        else if(this.internal.isClosed())
        {
            builder.append("closed/unusable");
        }
        else if(this.internal.pBrowser == 0)
        {
            builder.append("internally uninitialized/unusable");
        }
        else if(this.internal.interfaces == null)
        {
            builder.append("pending population, internal pointer = 0x").
                    append(Long.toHexString(this.internal.pBrowser));
        }
        else
        {
            builder.append("contains info on ").append(this.internal.interfaces.size()).append(" interfaces").
                    append(", internal pointer = 0x").append(Long.toHexString(this.internal.pBrowser));
        }
        builder.append(">");
        return builder.toString();
    }

    private static final class InterfaceIterator implements Iterator<Interface>
    {
        private final Iterator<Interface> iterator;

        InterfaceIterator(final Iterator<Interface> iterator)
        {
            this.iterator = iterator;
        }

        @Override
        public boolean hasNext()
        {
            return this.iterator.hasNext();
        }

        @Override
        public Interface next()
        {
            return this.iterator.next();
        }
    }

    private static final class InterfaceEnumeration implements Enumeration<Interface>
    {
        private final Iterator<Interface> iterator;

        InterfaceEnumeration(final Iterator<Interface> iterator)
        {
            this.iterator = iterator;
        }

        @Override
        public boolean hasMoreElements()
        {
            return this.iterator.hasNext();
        }

        @Override
        public Interface nextElement()
        {
            return this.iterator.next();
        }
    }

    private static final class NativeBrowser implements AutoCloseable
    {
        /**
         * Stores the C++ native pointer to the OddSource::Interfaces::InterfaceBrowser object, used
         * internally by native code.
         */
        @SuppressWarnings("unused") // it is used by native code
        private long pBrowser;

        /**
         * Stores the cached Java conversions of the native interface info, set internally by native code.
         */
        @SuppressWarnings("unused") // it is used by native code
        private List<Interface> interfaces;

        private boolean closed;

        NativeBrowser()
        {
            this.init();
        }

        private synchronized native void init();

        public synchronized boolean isClosed()
        {
            return this.closed;
        }

        @Override
        public synchronized void close()
        {
            if (!this.closed)
            {
                this.closed = true;
                this.closeInternal();
            }
        }

        private native void closeInternal();

        synchronized native Interface getInterface(String name);

        synchronized native Interface getInterface(int index);

        synchronized List<Interface> getInterfaces()
        {
            this.ensureInterfacesPopulated();
            return this.interfaces;
        }

        private synchronized native void ensureInterfacesPopulated();

        synchronized boolean forEachInterface(final Function<Interface, Boolean> doThis)
        {
            if (this.interfaces != null)
            {
                for (final var iface : this.interfaces)
                {
                    if (!doThis.apply(iface))
                    {
                        return false;
                    }
                }
                return true;
            }

            return this.forEachInterfaceAndPopulate(doThis);
        }

        synchronized native boolean forEachInterfaceAndPopulate(Function<Interface, Boolean> doThis);
    }

    private static final class BrowserCleaner implements Runnable
    {
        private final NativeBrowser subject;

        BrowserCleaner(final NativeBrowser subject)
        {
            this.subject = subject;
        }

        @Override
        public void run()
        {
            this.subject.close();
        }
    }
}
