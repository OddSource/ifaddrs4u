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
 * Browse network interfaces/adapters configured on this system. To ensure resources are cleaned
 * up, be sure to {@link #close()} the browser when you are done with it.
 *
 * @since 1.0.0
 */
public final class InterfaceBrowser implements AutoCloseable, Iterable<Interface>
{
    static
    {
        if (!Loader.LOADED)
        {
            // this will never actually happen, but we need to ensure that Loader initializes
            throw new RuntimeException("Not possible!");
        }
    }

    private static final Cleaner cleaner = Cleaner.create();

    private final BrowserCleaner browserCleaner;

    private final Extern extern;

    private boolean closed;

    /**
     * Construct an interface browser.
     */
    public InterfaceBrowser()
    {
        this.browserCleaner = new BrowserCleaner();
        InterfaceBrowser.cleaner.register(this, this.browserCleaner);

        this.extern = new Extern();
        this.extern.init();
    }

    @Override
    public synchronized void close()
    {
        if(this.extern != null && !this.closed)
        {
            this.closed = true;
            this.extern.close();
        }
    }

    private synchronized void ready()
    {
        if(this.extern == null || this.closed)
        {
            throw new IllegalStateException("This interface browser has been closed or is in an unusable state.");
        }
    }

    /**
     * Get the interface with the given name, or {@code null} if no such interface exists. On Windows,
     * an attempt will be made to match the given name against the friendly name, the UUID, and the
     * UUID with curly braces around it. The complexity of this method is {@code O(n)} on the first call
     * to any methods on this instance and then {@code O(1)} for all subsequent calls.
     *
     * @param name The interface name or, optionally on Windows (only), the UUID or <code>{UUID}</code>
     * @return the matched interface or {@code null}.
     */
    public Interface getInterface(final String name)
    {
        this.ready();
        return this.extern.getInterface(name);
    }

    /**
     * Get the interface with the given index, or {@code null} if no such interface exists. The complexity
     * of this method is {@code O(n)} on the first call to any methods on this instance and then {@code O(1)}
     * for all subsequent calls.
     *
     * @param index The interface index
     * @return the matched interface or {@code null}.
     */
    public Interface getInterface(final int index)
    {
        this.ready();
        return this.extern.getInterface(index);
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
        return new InterfaceEnumeration(this.extern.getInterfaces().listIterator());
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
        return new InterfaceIterator(this.extern.getInterfaces().listIterator());
    }

    /**
     * Get a filterable stream of all interfaces configured on the system.<br>
     * <br>
     * Note that there is no guaranteed order in which interfaces will be streamed, and that the
     * order can change from one call to the next (though that would not be typical). If you need
     * a particular order, consider sorting this stream.
     *
     * @return a stream of interfaces.
     */
    public Stream<Interface> interfaces()
    {
        this.ready();
        return this.extern.getInterfaces().stream();
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
        return this.extern.forEachInterface(doThis);
    }

    @Override
    public String toString()
    {
        final var builder = new StringBuilder("<io.oddsource.java.net.ifaddrs4j.InterfaceBrowser: ");
        if (this.extern == null)
        {
            builder.append("uninitialized/unusable");
        }
        else if(this.closed)
        {
            builder.append("closed/unusable");
        }
        else if(this.extern.pBrowser == 0)
        {
            builder.append("internally uninitialized/unusable");
        }
        else if(this.extern.interfaces == null)
        {
            builder.append("pending population, internal pointer = 0x").append(Long.toHexString(this.extern.pBrowser));
        }
        else
        {
            builder.append("contains info on ").append(this.extern.interfaces.size()).append(" interfaces");
            builder.append(", internal pointer = 0x").append(Long.toHexString(this.extern.pBrowser));
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

    private static final class Extern implements AutoCloseable
    {
        /**
         * Stores the C++ native pointer to the OddSource::Interfaces::InterfaceBrowser object, used
         * internally by native code.
         */
        private long pBrowser;

        /**
         * Stores the cached Java conversions of the native interface info, set internally by native code.
         */
        private List<Interface> interfaces;

        Extern()
        {
        }

        native void init();

        @Override
        public native void close();

        native Interface getInterface(final String name);

        native Interface getInterface(final int index);

        List<Interface> getInterfaces()
        {
            this.ensureInterfacesPopulated();
            return this.interfaces;
        }

        private native void ensureInterfacesPopulated();

        boolean forEachInterface(final Function<Interface, Boolean> doThis)
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

        native boolean forEachInterfaceAndPopulate(final Function<Interface, Boolean> doThis);
    }

    private final class BrowserCleaner implements Runnable
    {
        BrowserCleaner()
        {
        }

        @Override
        public void run()
        {
            InterfaceBrowser.this.close();
        }
    }
}
