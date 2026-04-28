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

#ifndef IFADDRS4CPP_INLINE_SOURCE
#include "../Interfaces.hpp"
#endif /* IFADDRS4CPP_INLINE_SOURCE */

#ifdef ODDSOURCE_IS_WINDOWS

#include "winsock_includes.h"

#else /* ODDSOURCE_IS_WINDOWS */

#include <cerrno>
#include <cstring>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>

#if __has_include(<linux/ipv6.h>)
#include <linux/ipv6.h>
#define HAS_LINUX_IPV6
#endif /* <linux/ipv6.h> */

#if __has_include(<net/if_arp.h>)
#include <net/if_arp.h>
#endif

#if __has_include(<net/if_dl.h>)
#include <net/if_dl.h>
#elif __has_include(<linux/if_packet.h>)
#include <linux/if_packet.h>
#else
#error "Either net/if_dl.h or linux/if_packet.h must be present on the system"
#endif /* !<net/if_dl.h> && !<linux/if_packet.h> */

#endif /* !ODDSOURCE_IS_WINDOWS */

#include <cassert>
#include <functional>
#include <iomanip>
#include <sstream>
#include <string>

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
    void
    populateInterfaces(
        ::std::list< ::std::string > & warnings,
        ::std::list< ::std::shared_ptr< Interface const > > & interfaces,
        ::std::function< void( Interface &, MacAddress && ) > setMacAddress,
        ::std::function< void( Interface &, InterfaceIPv4Address && ) > addIPv4Address,
        ::std::function< void( Interface &, InterfaceIPv6Address && ) > addIPv6Address );
}

namespace OddSource::Interfaces
{
    OddSource_Inline
    InterfaceBrowserSystemError::
    InterfaceBrowserSystemError(
        ::std::string_view const & what )
        : ::std::runtime_error( ::std::string( what ) )
    {
    }

    OddSource_Inline
    InterfaceBrowserSystemError::
    InterfaceBrowserSystemError(
        InterfaceBrowserSystemError const & other ) noexcept
        : ::std::runtime_error( other )
    {
    }

    OddSource_Inline
    InterfaceBrowserSystemError::
    ~InterfaceBrowserSystemError() noexcept
    {
    }

#ifdef ODDSOURCE_IS_WINDOWS
    WinSockStartupCleanupHelper::
    WinSockStartupCleanupHelper()
    {
        using namespace ::std::string_literals;
        WORD version_requested = MAKEWORD( 2, 2 );
        WSADATA data;
        int error = WSAStartup( version_requested, &data );
        if (error != 0)
        {
            throw ::std::runtime_error(
                "Could not initialize WinSock subsystem due to error code: "s +
                ::std::to_string(error) + ". For the meaning of this, see the documentation: "s +
                "https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-wsastartup#return-value"s);
        }
    }

    WinSockStartupCleanupHelper::
    ~WinSockStartupCleanupHelper()
    {
        WSACleanup();
    }
#endif /* ODDSOURCE_IS_WINDOWS */

    OddSource_Inline
    InterfaceBrowser::
    InterfaceBrowser()
    {
        populateInterfaces(
            this->_warnings,
            this->_interfaces,
            []( Interface & rInterface, MacAddress && macAddress )
            { rInterface._macAddress.emplace( std::move( macAddress ) ); },
            []( Interface & rInterface, InterfaceIPv4Address && ipAddress )
            { rInterface._ipv4Addresses.push_back( std::move( ipAddress ) ); },
            []( Interface & rInterface, InterfaceIPv6Address && ipAddress )
            { rInterface._ipv6Addresses.push_back( std::move( ipAddress ) ); } );

        for ( auto const & pInterface : this->_interfaces )
        {
            this->_indexToInterface.emplace( pInterface->index(), pInterface );
            this->_nameToInterface.emplace( pInterface->name(), pInterface );
#ifdef ODDSOURCE_IS_WINDOWS
            if ( pInterface->name() != pInterface->description() &&
                 this->_nameToInterface.find( pInterface->description() ) == this->_nameToInterface.end() )
            {
                this->_nameToInterface.emplace( pInterface->description(), pInterface );
            }
#endif /* ODDSOURCE_IS_WINDOWS */
        }
    }

    OddSource_Inline
    InterfaceBrowser::
    ~InterfaceBrowser() noexcept
    {
    }

    OddSource_Inline
    bool
    InterfaceBrowser::
    for_each_interface(
        ::std::function< bool( Interface const & ) > doThis ) const
    {
        return ::std::all_of(
            this->_interfaces.begin(),
            this->_interfaces.end(),
            [ &doThis ]
            ( ::std::shared_ptr< Interface const > const & pInterface ) -> bool
            { return doThis( *pInterface ); } );
    }

    OddSource_Inline
    ::std::list< ::std::shared_ptr< Interface const > > const &
    InterfaceBrowser::
    get_interfaces() const
    {
        return this->_interfaces;
    }

    OddSource_Inline
    ::std::shared_ptr< Interface const >
    InterfaceBrowser::
    get_interface(
        ::std::uint32_t index ) const
    {
        if ( auto const found( this->_indexToInterface.find( index ) ); found != this->_indexToInterface.end() )
        {
            return found->second;
        }
        return nullptr;
    }

    OddSource_Inline
    Interface const &
    InterfaceBrowser::
    operator[](
        ::std::uint32_t index ) const
    {
        using namespace std::string_literals;
        if ( auto const found( this->_indexToInterface.find( index ) ); found != this->_indexToInterface.end() )
        {
            return *found->second;
        }
        throw ::std::out_of_range( "interface index not found: "s + std::to_string( index ) );
    }

    OddSource_Inline
    ::std::shared_ptr< Interface const >
    InterfaceBrowser::
    get_interface(
        ::std::string_view name ) const
    {
        if ( auto const found( this->_nameToInterface.find( std::string( name ) ) );
             found != this->_nameToInterface.end() )
        {
            return found->second;
        }
        return nullptr;
    }

    OddSource_Inline
    Interface const &
    InterfaceBrowser::
    operator[](
        ::std::string_view name ) const
    {
        using namespace std::string_literals;
        if ( auto const found( this->_nameToInterface.find( std::string( name ) ) );
             found != this->_nameToInterface.end() )
        {
            return *found->second;
        }
        throw ::std::out_of_range( "interface name not found: "s + std::string( name ) );
    }

    OddSource_Inline
    ::std::list< ::std::string > const &
    InterfaceBrowser::
    getWarnings() const
    {
        return this->_warnings;
    }
}

#ifndef ODDSOURCE_IS_WINDOWS

#  ifdef AF_LINK
#    define ODDSOURCE_AF_MAC_ADDRESS AF_LINK
#  else /* AF_LINK */
#    define ODDSOURCE_AF_MAC_ADDRESS AF_PACKET
#  endif /* AF_LINK */

#  if defined(__clang__) && ODDSOURCE_IS_MACOS
#    define STRERROR_R_RETURNS_INT 1
#  elif ( _POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600 ) && !_GNU_SOURCE
#    define STRERROR_R_RETURNS_INT 1
#  else
#    define STRERROR_R_RETURNS_INT 0
#  endif

#endif /* ODDSOURCE_IS_WINDOWS */

// ReSharper disable once CppUnnamedNamespaceInHeaderFile
namespace
{
#ifdef ODDSOURCE_IS_WINDOWS
    ::std::string
    utf8Encode(
        ::std::wstring const & wideString )
    {
        if ( wideString.empty() )
        {
            return ::std::string();
        }
        int sizeNeeded{ ::WideCharToMultiByte(
            CP_UTF8,
            0,
            &wideString[ 0 ],
            static_cast< int >( wideString.size() ),
            NULL,
            0,
            NULL,
            NULL ) };
        ::std::string string( sizeNeeded, 0 );
        ::WideCharToMultiByte(
            CP_UTF8,
            0,
            &wideString[ 0 ],
            static_cast< int >( wideString.size() ),
            &string[ 0 ],
            sizeNeeded,
            NULL,
            NULL);
        return string;
    }

    /*class InterfacePopulator
    {
    private:
        PIP_ADAPTER_ADDRESSES _pAddresses;

    public:
        InterfacePopulator()
          : _pAddresses( nullptr )
        {
        }

        ~InterfacePopulator()
        {
            if ( this->_pAddresses )
            {
                FREE( this->_pAddresses );
            }
        }
    };*/

    int
    getSystemErrorCode()
    {
        int errorCode( ::GetLastError() );
        if ( errorCode == 0 )
        {
            errorCode = ::WSAGetLastError();
        }
        return errorCode;
    }

    ::std::string
    getSystemErrorMessage(
        int const errorCode )
    {
        char * errorMessageBuffer( nullptr );
        DWORD const result( ::FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            errorCode,
            MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
            (LPSTR)&errorMessageBuffer,
            0,
            nullptr ) );

        ::std::string errorMessage;
        if ( result > 0 )
        {
            errorMessage = errorMessageBuffer;
        }
        ::LocalFree( errorMessageBuffer );

        using namespace ::std::string_literals;
        return errorMessage.empty() ? "Unknown error"s : errorMessage;
    }

    PIP_ADAPTER_ADDRESSES
    allocateAdapterAddresses(
        ULONG const bufferLength )
    {
        HANDLE heap( ::GetProcessHeap() );
        if ( heap == nullptr )
        {
            auto const errorCode( getSystemErrorCode() );
            ::std::ostringstream oss;
            oss << "Error " << errorCode << " getting process heap handle to allocate adapter addresses: "
                << getSystemErrorMessage( errorCode );
            throw InterfaceBrowserSystemError( oss.str() );
        }

        ::std::string errorMessage;
        LPVOID allocation( nullptr )
        __try
        {
            allocation = ::HeapAlloc( heap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, bufferLength );
        }
        __except ( EXCEPTION_EXECUTE_HANDLER )
        {
            DWORD const errorCode{ GetExceptionCode() };
            ::std::ostringstream oss;
            oss << "Failed to allocate " << bufferLength << " bytes for adapter addresses ";
            if ( errorCode == STATUS_NO_MEMORY )
            {
                oss << "due to a lack of sufficient memory resources.";
            }
            else if ( errorCode == STATUS_ACCESS_VIOLATION )
            {
                oss << "due to heap corruption or invalid access.";
            }
            else
            {
                oss << "due to unknown error code: 0x" << ::std::hex << errorCode << ::std::dec << ".";
            }
        }

        if ( errorMessage )
        {
            throw InterfaceBrowserSystemError( errorMessage );
        }
        else if ( allocation == nullptr )
        {
            throw InterfaceBrowserSystemError( "Failed to allocate adapter addresses due to unknown error." );
        }

        return reinterpret_cast< PIP_ADAPTER_ADDRESSES >( allocation );
    }

    void
    freeIpAdapterAddresses(
        PIP_ADAPTER_ADDRESSES addresses )
    {
        HANDLE heap( ::GetProcessHeap() );
        if ( heap == nullptr )
        {
            auto const errorCode( getSystemErrorCode() );
            ::std::ostringstream oss;
            oss << "Error " << errorCode << " getting process heap handle to free adapter addresses allocation: "
                << getSystemErrorMessage( errorCode );
            throw InterfaceBrowserSystemError( oss.str() );
        }
        if ( !::HeapFree( heap, 0, addresses ) )
        {
            auto const errorCode( getSystemErrorCode() );
            ::std::ostringstream oss;
            oss << "Error " << errorCode << " freeing adapter addresses allocation: "
                << getSystemErrorMessage( errorCode );
            throw InterfaceBrowserSystemError( oss.str() );
        }
    }

    void
    _addIPv4Address(
        ::std::list< ::std::string > & /* warnings */,
        ::std::function< void( Interface &, InterfaceIPv4Address && ) > addIPv4Address,
        Interface & rInterface,
        bool & isBroadcast,
        LPSOCKADDR sa,
        PIP_ADAPTER_PREFIX pre,
        ::std::uint8_t const prefixLength = 0 )
    {
        auto addr( reinterpret_cast< sockaddr_in * >( sa ) );
        IPv4Address const address( &addr->sin_addr );

        sockaddr_in * broadcast = nullptr;
        while ( pre )
        {
            LPSOCKADDR candidate( pre->Address.lpSockaddr );
            if ( candidate->sa_family == AF_INET )
            {
                auto cand( reinterpret_cast< sockaddr_in * >( candidate ) );
                auto candBytes( reinterpret_cast< ::std::uint8_t const * >( &cand->sin_addr.s_addr ) );
                ::std::uint8_t firstByteWithBroadcast{ 0 };
                for ( int i{ 3 }; i >= 0; --i)
                {
                    if ( candBytes[ i ] == 0xff )
                    {
                        firstByteWithBroadcast = i;
                    }
                }
                if ( firstByteWithBroadcast > 0 )
                {
                    auto addrBytes( reinterpret_cast< ::std::uint8_t const * >( &addr->sin_addr.s_addr ) );
                    bool unmatch{ false };
                    for ( int i{ 0 }; i < firstByteWithBroadcast; ++i)
                    {
                        if ( candBytes[ i ] != addrBytes[ i ])
                        {
                            unmatch = true;
                            break;
                        }
                    }
                    if ( !unmatch )
                    {
                        broadcast = cand;
                        isBroadcast = true;
                        break;
                    }
                }
            }

            pre = pre->Next;
        }

        static constexpr ::std::uint32_t const flags{ 0 };
        if ( broadcast )
        {
            addIPv4Address( rInterface, InterfaceIPv4Address(
                address,
                flags,
                prefixLength,
                Broadcast,
                IPv4Address( &broadcast->sin_addr ) ) );
        }
        else
        {
            addIPv4Address( rInterface, InterfaceIPv4Address(
                address,
                flags,
                prefixLength );
        }
    }

    void
    _addIPv6Address(
        ::std::list< ::std::string > & /* warnings */,
        ::std::function< void( Interface &, InterfaceIPv6Address && ) > addIPv6Address,
        Interface & rInterface,
        LPSOCKADDR sa,
        ::std::uint8_t const prefixLength,
        ::std::uint16_t const flags )
    {
        auto addr( reinterpret_cast< sockaddr_in6 * >( sa ) );
        if ( addr->sin6_scope_id )
        {
            addIPv6Address( rInterface, InterfaceIPv6Address(
                IPv6Address( &addr->sin6_addr, addr->sin6_scope_id ),
                flags,
                prefixLength ) );
        }
        else
        {
            addIPv6Address( rInterface, InterfaceIPv6Address(
                IPv6Address( &addr->sin6_addr ),
                flags,
                prefixLength ) );
        }
    }

    void
    populateInterfaces(
        ::std::list< ::std::string > & warnings,
        ::std::list< ::std::shared_ptr< Interface const > > & interfaces,
        ::std::function< void( Interface &, MacAddress && ) > setMacAddress,
        ::std::function< void( Interface &, InterfaceIPv4Address && ) > addIPv4Address,
        ::std::function< void( Interface &, InterfaceIPv6Address && ) > addIPv6Address )
    {
        using namespace ::std::string_literals;

        ::std::unique_ptr< IP_ADAPTER_ADDRESSES, decltype( &freeIpAdapterAddresses ) > pAdapterAddresses(
            nullptr,
            &freeIpAdapterAddresses );

        {
            DWORD result{ 0 };
            ULONG iterations{ 0 };
            ULONG const flags{ GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER };
            ULONG bufferLength{ sizeof( IP_ADAPTER_ADDRESSES ) * 30 };

            do
            {
                pAdapterAddresses.reset( allocateAdapterAddresses( bufferLength ) );
                if ( !pAdapterAddresses )
                {
                    throw InterfaceBrowserSystemError(
                        "Adapter addresses unexpectedly a null pointer before call to GetAdapterAddresses." );
                }

                result = ::GetAdaptersAddresses( AF_UNSPEC, flags, nullptr, pAdapterAddresses, &bufferLength );
                iterations++;

                if ( result == ERROR_BUFFER_OVERFLOW )
                {
                    pAdapterAddresses.reset();
                }
            }
            while ( result == ERROR_BUFFER_OVERFLOW && iterations < ODDSOURCE_MAX_TRIES );

            if ( result == ERROR_NO_DATA )
            {
                throw InterfaceBrowserSystemError( "GetAdapterAddresses returned no results." );
            }
            if ( result != NO_ERROR )
            {
                ::std::ostringstream oss;
                oss << "Error " << result << " calling GetAdapterAddresses: " << getSystemErrorMessage( result );
                throw InterfaceBrowserSystemError( oss.str() );
            }
        }

        if ( !pAdapterAddresses )
        {
            throw InterfaceBrowserSystemError(
                "Adapter addresses unexpectedly a null pointer after call to GetAdapterAddresses." );
        }

        for ( PIP_ADAPTER_ADDRESSES pIfAddr = pAdapterAddresses.get(); pIfAddr != nullptr; pIfAddr = pIfAddr->Next )
        {
            ::std::uint16_t flags{ 0 };
            if ( pIfAddr->IfType == IF_TYPE_SOFTWARE_LOOPBACK )
            {
                flags |= InterfaceFlag::IsLoopback;
            }
            if ( pIfAddr->OperStatus == IfOperStatusUp )
            {
                flags |= InterfaceFlag::IsUp;
                flags |= InterfaceFlag::IsRunning;
            }
            if ( ( pIfAddr->Flags & IP_ADAPTER_NO_MULTICAST ) != IP_ADAPTER_NO_MULTICAST )
            {
                flags |= InterfaceFlag::SupportsMulticast;
            }

            ::std::uint32_t const index{ pIfAddr->IfIndex == 0 ? pIfAddr->Ipv6IfIndex : pIfAddr->IfIndex };
            assert( index > 0 );

            ::std::string const guid( pIfAddr->AdapterName );
            auto pInterface( ::std::make_shared< Interface >(
                index,
                guid,
                utf8Encode( ::std::wstring( pIfAddr->FriendlyName ) ),
                utf8Encode( ::std::wstring( pIfAddr->Description ) ),
                flags,
                pIfAddr->Mtu ) );
            interfaces.emplace_back( pInterface );

            if ( pIfAddr->PhysicalAddress && pIfAddr->PhysicalAddressLength > 0 )
            {
                setMacAddress( pInterface, MacAddress(
                    pIfAddr->PhysicalAddress,
                    static_cast< ::std::uint8_t >( pIfAddr->PhysicalAddressLength ) ) );
            }

            for (
                PIP_ADAPTER_UNICAST_ADDRESS pUnicastAddr = pIfAddr->FirstUnicastAddress;
                pUnicastAddr != nullptr;
                pUnicastAddr = pUnicastAddr->Next )
            {
                LPSOCKADDR sa( pUnicastAddr->Address.lpSockaddr );
                if ( sa->sa_family == AF_INET )
                {
                    bool isBroadcast{ false };
                    _addIPv4Address(
                        warnings,
                        addIPv4Address,
                        *pInterface,
                        isBroadcast,
                        sa,
                        pIfAddr->FirstPrefix,
                        pUnicastAddr->OnLinkPrefixLength );

                    if ( isBroadcast )
                    {
                        pInterface->_flags |= InterfaceFlag::BroadcastAddressSet;
                    }
                }
                else if ( sa->sa_family == AF_INET6 )
                {
                    ::std::uint16_t flags{ 0 };
                    if ( pUnicastAddr->SuffixOrigin == IpSuffixOriginRandom )
                    {
                        flags |= InterfaceIPAddressFlag::Temporary;
                    }
                    _addIPv6Address(
                        warnings,
                        addIPv6Address,
                        *pInterface,
                        sa,
                        unicast->OnLinkPrefixLength,
                        flags );
                }
                else
                {
                    ::std::ostringstream oss;
                    oss << "Unrecognized unicast address family " << ::std::to_string( sa->sa_family )
                        << " on interface " << guid;
                    warnings.push_back( oss.str() );
                }
            }

            for (
                PIP_ADAPTER_ANYCAST_ADDRESS pAnycastAddr = pIfAddr->FirstAnycastAddress;
                pAnycastAddr != nullptr;
                pAnycastAddr = pAnycastAddr->Next )
            {
                LPSOCKADDR sa = pAnycastAddr->Address.lpSockaddr;
                if ( sa->sa_family == AF_INET )
                {
                    // Extremely unlikely, as IPv4 doesn't natively support Anycast
                    // (works only with BGP), but it's Windows, so there's no telling.
                    bool isBroadcast{ false };
                    _addIPv4Address(
                        warnings,
                        addIPv4Address,
                        *pInterface,
                        isBroadcast,
                        sa,
                        pIfAddr->FirstPrefix );

                    ::std::ignore = isBroadcast;
                }
                else if ( sa->sa_family == AF_INET6 )
                {
                    _addIPv6Address(
                        warnings,
                        addIPv6Address,
                        *pInterface,
                        sa,
                        0,
                        0 | InterfaceIPAddressFlag::Anycast );
                }
                else
                {
                    ::std::ostringstream oss;
                    oss << "Unrecognized anycast address family " << ::std::to_string( sa->sa_family )
                        << " on interface " << guid;
                    warnings.push_back( oss.str() );
                }
            }
        }
    }

#else /* ODDSOURCE_IS_WINDOWS */

    /*class InterfacePopulator
    {
    private:
        struct ifaddrs * _ifaddrs;

    public:
        InterfacePopulator()
          : _ifaddrs( nullptr )
        {
        }

        ~InterfacePopulator()
        {
            if ( this->_ifaddrs )
            {
                ::freeifaddrs( this->_ifaddrs );
            }
        }
    };*/

    ::std::string
    getSystemErrorMessage(
        int const errorCode )
    {
        static constexpr size_t const MAX_LENGTH{ 4096 };
        char errorMessageBuffer[ MAX_LENGTH ];
#if STRERROR_R_RETURNS_INT == 1
        ::std::string errorMessage;
        if ( int const result( ::strerror_r( errorCode, errorMessageBuffer, MAX_LENGTH ) ); result == 0 )
        {
            errorMessage = errorMessageBuffer;
        }
#else
        ::std::string const errorMessage( ::strerror_r( errorCode, errorMessageBuffer, MAX_LENGTH ) );
#endif

        using namespace ::std::string_literals;
        return errorMessage.empty() ? "Unknown error"s : errorMessage;
    }

    ::std::optional< ::std::uint64_t const >
    getMtu(
        ::std::list< ::std::string > & warnings,
        char const * interfaceName )
    {
        ::std::optional< ::std::uint64_t const > mtu;
#ifdef SIOCGIFMTU

        ifreq ifr {};
        ::strncpy( ifr.ifr_name, interfaceName, IFNAMSIZ - 1 );
        if ( int sock( ::socket( AF_INET, SOCK_DGRAM, 0 ) ); sock > -1 )
        {
            if ( ::ioctl( sock, SIOCGIFMTU, &ifr ) >= 0 )
            {
                mtu.emplace( ifr.ifr_mtu );
            }
            else
            {
                auto const errorCode( errno );
                ::std::ostringstream oss;
                oss << "Error " << errorCode << " calling ioctl on datagram socket to determine MTU for interface "
                    << interfaceName << ": " << getSystemErrorMessage( errorCode );
                warnings.push_back( oss.str() );
            }
        }
        else
        {
            auto const errorCode( errno );
            ::std::ostringstream oss;
            oss << "Error " << errorCode << " creating datagram socket to determine MTU for interface "
                << interfaceName << ": " << getSystemErrorMessage( errorCode );
            warnings.push_back( oss.str() );
        }

#endif
        return mtu;
    }

    void
    _setMacAddress(
        ::std::list< ::std::string > & warnings,
        ::std::function< void( Interface &, MacAddress && ) > const & setMacAddress,
        Interface & rInterface,
        struct ifaddrs const * pIfAddr )
    {
#ifdef AF_LINK

        assert( pIfAddr->ifa_addr->sa_family == AF_LINK );
        auto const addr( reinterpret_cast< sockaddr_dl * >( pIfAddr->ifa_addr ) );
        // LLADDR returns a signed char, not unsigned, but the data itself is
        // actually unsigned. (This is because addr->sdl_data contains two sets
        // of data in a signed char, and LLADDR merely extracts the one we need.)
        // So we have to cast to unsigned in order to make use of the data.
        auto const dataLength{ addr->sdl_alen }; // should always be 6, but you never know
        if ( dataLength < MIN_ADAPTER_ADDRESS_LENGTH )
        {
            ::std::ostringstream oss;
            oss << "Mac address for interface " << rInterface.name() << " had unexpected length " << dataLength
                << " bytes, should be at least " << MIN_ADAPTER_ADDRESS_LENGTH << " bytes.";
            warnings.push_back( oss.str() );
            return;
        }
        auto const data( reinterpret_cast< ::std::uint8_t const * >( LLADDR( addr ) ) );

#else /* AF_LINK */

        assert( pIfAddr->ifa_addr->sa_family == AF_PACKET );
        auto const addr( reinterpret_cast< sockaddr_ll * >( pIfAddr->ifa_addr ) );
        if ( addr->sll_hatype != ARPHRD_ETHER )
        {
            ::std::ostringstream oss;
            oss << "Mac address for interface " << rInterface.name() << " had unexpected HA type " << addr->sll_hatype
                << ", should be " << ARPHRD_ETHER << ".";
            warnings.push_back( oss.str() );
            return;
        }
        auto const dataLength{ addr->sll_halen }; // should always be 6, but you never know
        if ( dataLength < MIN_ADAPTER_ADDRESS_LENGTH )
        {
            ::std::ostringstream oss;
            oss << "Mac address for interface " << rInterface.name() << " had unexpected length " << dataLength
                << " bytes, should be at least " << MIN_ADAPTER_ADDRESS_LENGTH << " bytes.";
            warnings.push_back( oss.str() );
            return;
        }
        auto const data( addr->sll_addr );

#endif /* !AF_LINK */
        for ( ::std::uint8_t i{ 0 }; i < dataLength; i++ )
        {
            if ( data[ i ] > 0 )
            {
                // make sure at least one byte is nonzero
                setMacAddress( rInterface, MacAddress( data, dataLength ) );
                return;
            }
        }
        ::std::ostringstream oss;
        oss << "Mac address for interface " << rInterface.name() << " was unexpectedly all zeroes.";
        warnings.push_back( oss.str() );
    }

    void
    _addIPv4Address(
        ::std::list< ::std::string > & /* warnings */,
        ::std::function< void( Interface &, InterfaceIPv4Address && ) > const & addIPv4Address,
        Interface & rInterface,
        struct ifaddrs const * pIfAddr )
    {
        auto const addr( reinterpret_cast< sockaddr_in * >( pIfAddr->ifa_addr ) );
        IPv4Address const address( &addr->sin_addr );

        ::std::uint8_t prefixLength{ 0 };
        if ( pIfAddr->ifa_netmask )
        {
            auto netmaskAddr( reinterpret_cast< sockaddr_in * >( pIfAddr->ifa_netmask ) );
            auto netmask{ static_cast< ::std::uint32_t >( netmaskAddr->sin_addr.s_addr ) };
            while (netmask > 0)
            {
                netmask = netmask >> 1;
                prefixLength++;
            }
        }

        static constexpr ::std::uint32_t const flags{ 0 };
        if ( rInterface.is_flag_enabled( InterfaceFlag::BroadcastAddressSet ) && pIfAddr->ifa_broadaddr )
        {
            auto broadcastAddress( reinterpret_cast< sockaddr_in * >( pIfAddr->ifa_broadaddr ) );
            addIPv4Address( rInterface, InterfaceIPv4Address(
                address,
                flags,
                prefixLength,
                Broadcast,
                IPv4Address( &broadcastAddress->sin_addr ) ) );
        }
        else if( rInterface.is_flag_enabled( InterfaceFlag::IsPointToPoint ) && pIfAddr->ifa_dstaddr )
        {
            auto pointToPointDestination( reinterpret_cast< sockaddr_in * >( pIfAddr->ifa_dstaddr ) );
            addIPv4Address( rInterface, InterfaceIPv4Address(
                address,
                flags,
                prefixLength,
                PointToPoint,
                IPv4Address( &pointToPointDestination->sin_addr ) ) );
        }
        else
        {
            addIPv4Address( rInterface, InterfaceIPv4Address( address, flags, prefixLength ) );
        }
    }

    void
    _addIPv6Address(
        ::std::list< ::std::string > & warnings,
        ::std::function< void( Interface &, InterfaceIPv6Address && ) > const & addIPv6Address,
        Interface & rInterface,
        ::std::unordered_map< ::std::uint32_t, ::std::string > const & indexToName,
        struct ifaddrs const * pIfAddr )
    {
        auto addr( reinterpret_cast< sockaddr_in6 * >( pIfAddr->ifa_addr ) );
        ::std::optional< v6Scope > scope;
        ::std::optional< ::std::uint32_t > scopeId;
        if ( addr->sin6_scope_id )
        {
            scopeId = addr->sin6_scope_id;
            if ( auto found = indexToName.find( addr->sin6_scope_id ); found != indexToName.end() )
            {
                scope = { addr->sin6_scope_id, found->second };
            }
        }

        ::std::uint8_t prefixLength{ 0 };
        if ( pIfAddr->ifa_netmask )
        {
            auto netmask( reinterpret_cast< sockaddr_in6 * >( pIfAddr->ifa_netmask ) );
            auto netmaskBytes( static_cast< ::std::uint8_t * >( netmask->sin6_addr.s6_addr ) );
            ::std::uint8_t i{ 0 }, n;
            while ( i < 16 )
            {
                n = netmaskBytes[ i ];
                while ( n > 0 )
                {
                    if ( n & 1 )
                    {
                        prefixLength++;
                    }
                    n = n / 2;
                }
                i++;
            }
        }

#ifdef SIOCGIFAFLAG_IN6
        in6_ifreq ifr6 {};
        ::strncpy( ifr6.ifr_name, pIfAddr->ifa_name, IFNAMSIZ - 1 );
        ifr6.ifr_addr = *addr;
        ::std::uint32_t flags{ 0 };
        if ( int sock( ::socket( AF_INET6, SOCK_DGRAM, 0 ) ); sock > -1 )
        {
            if ( ::ioctl( sock, SIOCGIFAFLAG_IN6, &ifr6 ) >= 0 )
            {
                // IPv6 addresses can have their own flags in addition to the interface's flags,
                // but only some OSes give you access to them
                flags = ifr6.ifr_ifru.ifru_flags6;
            }
            else
            {
                auto const errorCode( errno );
                ::std::ostringstream oss;
                oss << "Error " << errorCode << " calling ioctl on datagram socket to determine flags for address "
                    << IPv6Address( &addr->sin6_addr ) << " on interface " << rInterface.name() << ": "
                    << getSystemErrorMessage( errorCode );
                warnings.push_back( oss.str() );
            }
        }
        else
        {
            auto const errorCode( errno );
            ::std::ostringstream oss;
            oss << "Error " << errorCode << " creating datagram socket to determine flags for address "
                << IPv6Address( &addr->sin6_addr ) << " on interface " << rInterface.name() << ": "
                << getSystemErrorMessage( errorCode );
            warnings.push_back( oss.str() );
        }
#else /* SIOCGIFAFLAG_IN6 */
        static constexpr ::std::uint32_t const flags{ 0 };
#endif /* !SIOCGIFAFLAG_IN6 */

        if ( scope )
        {
            addIPv6Address( rInterface, InterfaceIPv6Address(
                IPv6Address( &addr->sin6_addr, *scope ),
                flags,
                prefixLength ) );
        }
        else if ( scopeId )
        {
            addIPv6Address( rInterface, InterfaceIPv6Address(
                IPv6Address( &addr->sin6_addr, *scopeId ),
                flags,
                prefixLength ) );
        }
        else
        {
            addIPv6Address( rInterface, InterfaceIPv6Address(
                IPv6Address( &addr->sin6_addr ),
                flags,
                prefixLength ) );
        }
    }

    void
    populateInterfaces(
        ::std::list< ::std::string > & warnings,
        ::std::list< ::std::shared_ptr< Interface const > > & interfaces,
        ::std::function< void( Interface &, MacAddress && ) > setMacAddress,
        ::std::function< void( Interface &, InterfaceIPv4Address && ) > addIPv4Address,
        ::std::function< void( Interface &, InterfaceIPv6Address && ) > addIPv6Address )
    {
        ::std::unique_ptr< struct ifaddrs, decltype( &::freeifaddrs ) > pIfAddrs(
            nullptr,
            &::freeifaddrs );
        if ( struct ifaddrs * ifaddr; ::getifaddrs( &ifaddr ) == 0 )
        {
            pIfAddrs.reset( ifaddr );
        }
        else
        {
            auto const errorCode( errno );
            ::std::ostringstream oss;
            oss << "Error " << errorCode << " calling getifaddrs: " << getSystemErrorMessage( errorCode );
            throw InterfaceBrowserSystemError( oss.str() );
        }

        ::std::unordered_map< ::std::string, ::std::shared_ptr< Interface > > nameToInterface;
        ::std::unordered_map< ::std::uint32_t, ::std::string > indexToName;
        for ( struct ifaddrs * pIfAddr = pIfAddrs.get(); pIfAddr != nullptr; pIfAddr = pIfAddr->ifa_next )
        {
            ::std::string const name( pIfAddr->ifa_name );
            ::std::shared_ptr< Interface > pInterface;
            if ( auto const found = nameToInterface.find( name ); found != nameToInterface.end() )
            {
                pInterface = found->second;
                if ( pIfAddr->ifa_flags != pInterface->flags() )
                {
                    ::std::ostringstream oss;
                    oss << "Flags " << ::std::to_string( pIfAddr->ifa_flags ) << " for next item in interface "
                        << name << " does not match first flags " << ::std::to_string( pInterface->flags() );
                    warnings.push_back( oss.str() );
                }
            }
            else
            {
                ::std::uint32_t const index( ::if_nametoindex( pIfAddr->ifa_name ) );
                assert( index > 0 );
                pInterface = ::std::make_shared< Interface >(
                    index,
                    name,
                    name,
                    name,
                    pIfAddr->ifa_flags,
                    getMtu( warnings, pIfAddr->ifa_name ) );
                interfaces.emplace_back( pInterface );
                nameToInterface.emplace( name, pInterface );
                indexToName.emplace( index, name );
            }

            if ( pIfAddr->ifa_addr )
            {
                if ( AddressFamily const family( pIfAddr->ifa_addr->sa_family ); family == ODDSOURCE_AF_MAC_ADDRESS )
                {
                    _setMacAddress( warnings, setMacAddress, *pInterface, pIfAddr );
                }
                else if ( family == AF_INET )
                {
                    _addIPv4Address( warnings, addIPv4Address, *pInterface, pIfAddr );
                }
                else if ( family == AF_INET6 )
                {
                    _addIPv6Address( warnings, addIPv6Address, *pInterface, indexToName, pIfAddr );
                }
                else
                {
                    ::std::ostringstream oss;
                    oss << "Unrecognized address family " << ::std::to_string( family ) << " on interface " << name;
                    warnings.push_back( oss.str() );
                }
            }
        }
    }

#endif /* !ODDSOURCE_IS_WINDOWS */

    /*void
    populateInterfaces(
        ::std::list< ::std::shared_ptr< Interface const > > & interfaces )
    {
        InterfacePopulator populator;
        populator( interfaces );
    }*/
}

#ifndef ODDSOURCE_IS_WINDOWS

#  undef ODDSOURCE_AF_MAC_ADDRESS
#  undef STRERROR_R_RETURNS_INT

#endif /* ODDSOURCE_IS_WINDOWS */
