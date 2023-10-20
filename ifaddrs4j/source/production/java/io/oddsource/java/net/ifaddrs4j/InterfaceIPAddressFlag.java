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
 * Represents bitwise flags associated with the IP address's assignment to the interface.<br>
 * <br>
 * The bitwise flag values represented by the {@link #flag} field will differ from platform to platform,
 * and you must not assume their being the same from one platform to the next.<br>
 * <br>
 * Most of these flags are applicable only to IPv6, and on many platforms, none of them will ever be
 * set. This mostly exists to support the complex configuration options on *-BSD platforms, such as
 * Mac OS X, macOS and FreeBSD.
 *
 * @since 1.0.0
 */
public enum InterfaceIPAddressFlag
{
    /**
     * The address has been autoconfigured by the IPv6 subsystem (this is not the same as DHCPv6, which
     * is largely unused).
     */
    AutoConfigured(getConstant("AutoConfigured"), "autoconf"),
    /**
     * The address is deprecated and should not be used by any applications, but is still technically
     * assigned to the interface. If used, it is likely to not work beyond the link.
     */
    Deprecated(getConstant("Deprecated"), "deprecated"),
    /**
     * The address is cryptographically secure and should be preferred by most applications.
     */
    Secured(getConstant("Secured"), "secured"),
    /**
     * This is a temporary address and its use is not recommended unless no other addresses are available.
     */
    Temporary(getConstant("Temporary"), "temporary"),
    /**
     * This is an anycast address shared by more than one system. See
     * <a href="https://en.wikipedia.org/wiki/Anycast">https://en.wikipedia.org/wiki/Anycast</a>.
     */
    Anycast(getConstant("Anycast"), "anycast"),
    /**
     * This address has been detached from the link and will not work. Do not use it.
     */
    Detached(getConstant("Detached"), "detached"),
    /**
     * Duplicate Address Detection (DAD) has determined this is a duplicate address on the network (see
     * <a href="https://www.ibm.com/docs/en/zos/3.1.0?topic=discovery-duplicate-address-detection">
     * https://www.ibm.com/docs/en/zos/3.1.0?topic=discovery-duplicate-address-detection</a>).
     */
    Duplicated(getConstant("Duplicated"), "duplicated"),
    /**
     * This is a DHCPv6 address.
     */
    Dynamic(getConstant("Dynamic"), "dynamic"),
    /**
     * This is an Optimistic DAD address (see <a href="https://datatracker.ietf.org/doc/html/rfc4429">
     * https://datatracker.ietf.org/doc/html/rfc4429</a>).
     */
    Optimistic(getConstant("Optimistic"), "optimistic"),
    /**
     * This address's uniqueness on the link is still being verified.
     */
    Tentative(getConstant("Tentative"), "tentative"),
    /**
     * DAD has been disabled for this address.
     */
    NoDad(getConstant("NoDad"), "nodad");

    /**
     * The value used for bitwise operations.
     */
    public final int flag;

    /**
     * The string used to display this flag in {@link InterfaceIPAddress#toString()}.
     */
    public final String display;

    InterfaceIPAddressFlag(final int flag, final String display)
    {
        this.flag = flag;
        this.display = display;
    }

    private static int getConstant(final String name)
    {
        return EnumConstantsHelper.getInterfaceIPAddressFlagConstant(name);
    }
}
