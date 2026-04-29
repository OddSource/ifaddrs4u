/*
* Copyright © 2010-2026 OddSource Code (license@oddsource.io)
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

#pragma once

namespace OddSource::Interfaces
{
    inline
    IPAddressVersion
    IPv4Address::
    version() const noexcept
    {
        return IPAddressVersion::IPv4;
    }

    inline
    ::std::uint8_t
    IPv4Address::
    maximum_prefix_length() const noexcept
    {
        return 32;
    }

#ifdef ODDSOURCE_INCLUDE_BOOST
    inline
    IPv4Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4 >( *this ) );
    }

    inline
    IPv4Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v4( ntohl( this->_data->s_addr ) );
    }
#endif

    inline
    IPAddressVersion
    IPv6Address::
    version() const noexcept
    {
        return IPAddressVersion::IPv6;
    }

    OddSource_Inline
    ::std::uint8_t
    IPv6Address::
    maximum_prefix_length() const noexcept
    {
        return 128;
    }

#ifdef ODDSOURCE_INCLUDE_BOOST
    inline
    IPv6Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address() const
    {
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address(
            static_cast< ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6 >( *this ) )
    }

    inline
    IPv6Address::
    operator ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6() const
    {
        ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6::bytes_type bytes;
        // Copy the 16 bytes from in6_addr to bytes_type
        ::std::memcpy( bytes.data(), this->_data->s6_addr, 16 );
        // Construct and return the address_v6 object
        return ODDSOURCE_BOOST_NAMESPACE_ROOT::asio::ip::address_v6( bytes );
    }
#endif /* ODDSOURCE_INCLUDE_BOOST */
}
