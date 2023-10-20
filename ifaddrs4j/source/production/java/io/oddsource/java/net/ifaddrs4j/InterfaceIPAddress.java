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

import java.net.Inet6Address;
import java.net.InetAddress;
import java.util.Objects;

/**
 * A wrapper around java.lang.InetAddress to include flags and other details about how
 * an address is assigned to an interface.
 *
 * @param <IPAddressT> Either an {@link java.net.Inet4Address} or an {@link Inet6Address}, indicating what
 *                     type of address is bound to the interface in this representation.
 * @since 1.0.0
 */
public final class InterfaceIPAddress<IPAddressT extends InetAddress>
{
    private final IPAddressT address;

    private final Short prefixLength;

    private final IPAddressT broadcastAddress;

    private final IPAddressT pointToPointDestination;

    private final int flags;

    InterfaceIPAddress(final IPAddressT address, final int flags)
    {
        this(address, flags, null, null, null);
    }

    InterfaceIPAddress(final IPAddressT address, final int flags, final short prefixLength)
    {
        this(address, flags, prefixLength, null, null);
    }

    InterfaceIPAddress(
        final IPAddressT address,
        final int flags,
        final Short prefixLength,
        final IPAddressT broadcastAddress,
        final IPAddressT pointToPointDestination
    )
    {
        if(address == null)
        {
            throw new IllegalArgumentException("address cannot be null.");
        }
        if(broadcastAddress != null && pointToPointDestination != null)
        {
            throw new IllegalArgumentException(
                "Only one of broadcastAddress or pointToPointDestination may be non-null."
            );
        }
        this.address = address;
        this.flags = flags;
        this.prefixLength = prefixLength;
        this.broadcastAddress = broadcastAddress;
        this.pointToPointDestination = pointToPointDestination;
    }

    /**
     * The IP address.
     *
     * @return the IP address.
     */
    public IPAddressT getAddress()
    {
        return this.address;
    }

    /**
     * The prefix length, or {@code null} if there is no prefix length for this address.
     *
     * @return the prefix length or {@code null}.
     */
    public Short getPrefixLength()
    {
        return this.prefixLength;
    }

    /**
     * The broadcast address, or {@code null} if no broadcast address is set.
     *
     * @return the broadcast address or {@code null}.
     */
    public IPAddressT getBroadcastAddress()
    {
        return this.broadcastAddress;
    }

    /**
     * The point-to-point destination address, if this is a point-to-point interface and
     * the address is a point-to-point client address, or {@code null} otherwise.
     *
     * @return the point-to-point address or {@code null}.
     */
    public IPAddressT getPointToPointDestination()
    {
        return this.pointToPointDestination;
    }

    /**
     * Checks whether the indicated flag is enabled for this address. See the documentation for
     * {@link InterfaceIPAddressFlag} for more information.
     *
     * @param flag The flag to check
     * @return whether the flag is enabled.
     */
    public boolean isFlagEnabled(final InterfaceIPAddressFlag flag)
    {
        return (this.flags & flag.flag) == flag.flag;
    }

    public String toString()
    {
        final var builder = new StringBuilder(this.address.getHostAddress());
        if(this.prefixLength != null)
        {
            builder.append('/').append(this.prefixLength);
        }
        if(this.broadcastAddress != null)
        {
            builder.append(" broadcast ").append(this.broadcastAddress.getHostAddress());
        }
        else if(this.pointToPointDestination != null)
        {
            builder.append(" destination ").append(this.pointToPointDestination.getHostAddress());
        }
        if (this.flags > 0)
        {
            for (final var flag : InterfaceIPAddressFlag.values())
            {
                if((this.flags & flag.flag) == flag.flag)
                {
                    builder.append(' ').append(flag.display);
                }
            }
        }
        if(this.address instanceof Inet6Address)
        {
            final int scope_id = ((Inet6Address) this.address).getScopeId();
            if(scope_id > 0)
            {
                builder.append(" scopeid 0x").append(Long.toHexString(scope_id));
            }
        }
        return builder.toString();
    }

    public int hashCode()
    {
        return Objects.hash(
            this.address,
            this.prefixLength,
            this.broadcastAddress,
            this.pointToPointDestination,
            this.flags
        );
    }

    public boolean equals(final Object other)
    {
        if (!(other instanceof InterfaceIPAddress))
        {
            return false;
        }
        final var address = (InterfaceIPAddress<?>) other;
        return Objects.equals(this.address, address.address) &&
               Objects.equals(this.prefixLength, address.prefixLength) &&
               Objects.equals(this.broadcastAddress, address.broadcastAddress) &&
               Objects.equals(this.pointToPointDestination, address.pointToPointDestination) &&
               this.flags == address.flags;
    }
}
