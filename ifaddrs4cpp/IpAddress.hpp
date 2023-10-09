#ifdef IS_WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#else /* IS_WINDOWS */
#include <arpa/inet.h>
#include <cerrno>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#endif /* IS_WINDOWS */

#include <sstream>

template<typename Addr, typename>
const Addr *
OddSource::Interfaces::IPAddress::from_repr(::std::string_view const & repr)
{
    if (repr.length() == 0)
    {
        throw InvalidIPAddress("Invalid empty IP address string.");
    }

    size_t size;
    if constexpr (::std::is_same_v<Addr, in6_addr>)
    {
        size = 16;
    }
    else
    {
        size = 4;
    }

    auto data = new Addr[size];
    int success;
    if constexpr (::std::is_same_v<Addr, in6_addr>)
    {
        // inet_pton can also handle IPv4 addresses, but only in dotted-decimal format
        // (1.2.3.4), not in partial, hexadecimal, or any other valid IPv4 format.
        success = inet_pton(AF_INET6, ::std::string(repr).c_str(), data);
    }
    else
    {
        // inet_aton/inet_addr, however, can handle IPv4 addresses in all valid formats.
#ifdef IS_WINDOWS
        auto raw = inet_addr(repr);
        if (raw == INADDR_NONE)
        {
            success = 0;
        }
        else
        {
            ::std::memcpy(data, raw, sizeof(Addr));
        }
#else /* IS_WINDOWS */
        success = inet_aton(::std::string(repr).c_str(), data);
#endif
    }
    if (success != 1)
    {
        delete[] data;
        throw InvalidIPAddress("Malformed IP address string or unknown inet_*ton error.");
    }

    return data;
}

/*template<typename Addr,
         typename SockAddr,
         typename ::std::enable_if<(::std::is_same<in_addr, Addr>::value &&
                                    ::std::is_same<sockaddr_in, SockAddr>::value) ||
                                   (::std::is_same<in6_addr, Addr>::value &&
                                    ::std::is_same<sockaddr_in6, SockAddr>::value)>>*/
template<typename Addr, typename>
::std::string
OddSource::Interfaces::IPAddress::to_repr(const Addr * data)
{
    AddressFamily family;
    if constexpr (::std::is_same_v<Addr, in6_addr>)
    {
        family = AF_INET6;
    }
    else
    {
        family = AF_INET;
    }

    static const size_t host_length(100);
    char host_chars[host_length];
    auto ptr = ::inet_ntop(family, data, host_chars, sizeof(Addr));
    if (ptr == nullptr)
    {
#ifdef IS_WINDOWS
        auto err = ::WSAGetLastError();
#else /* IS_WINDOWS */
        auto err = ::gai_strerror(errno);
#endif /* IS_WINDOWS */
        throw InvalidIPAddress(
            (::std::ostringstream() << "Malformed IP address or inet_ntop system error: "
                                    << err).str()
        );
    }
    return host_chars;

    /*SockAddr sa;
    if constexpr (::std::is_same_v<SockAddr, sockaddr_in6>)
    {
        sa.sin6_family = AF_INET6;
        sa.sin6_addr = *data;
    }
    else
    {
        sa.sin_family = AF_INET;
        sa.sin_addr = *data;
    }

#ifdef IS_WINDOWS
    static const DWORD host_length(100);
    DWORD length(host_length);
    wchar_t host_wchars[host_length];
    int error = ::WSAAddressToString(sa, sizeof(SockAddr), nullptr, host_wchars, &length);
    if (error != 0)
    {
        throw OddSource::Interfaces::InvalidIPAddress(
            (::std::ostringstream() << "Malformed IP address or WSAAddressToString system error: "
                                    << ::WSAGetLastError()).str()
        );
    }
    if (length == 0)
    {
        throw OddSource::Interfaces::InvalidIPAddress("Malformed IP address yielded zero-length string");
    }
    ::std::wstring whost(host_wchars);
    ::std::string host = ::std::wstring_convert<::std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(whost);
#else // IS_WINDOWS
    static const size_t host_length(100);
    char host_chars[host_length];
    int error = ::getnameinfo(sa, sizeof(SockAddr), host_chars, host_length, 0, 0, NI_NUMERICHOST);
    if (error != 0)
    {
        throw OddSource::Interfaces::InvalidIPAddress(
            (::std::ostringstream() << "Malformed IP address or getnameinfo system error: "
                                    << ::gai_strerror(error)).str()
        );
    }
    ::std::string host(host_chars);
#endif // IS_WINDOWS

    return host; */
}

inline
OddSource::Interfaces::IPAddress::
operator ::std::string() const
{
    return this->_representation;
}

inline
OddSource::Interfaces::IPAddress::
operator char const *() const
{
    return this->_representation.c_str();
}

inline size_t
OddSource::Interfaces::IPv4Address::
data_length() const
{
    return 4;
}

inline
OddSource::Interfaces::IPv4Address::
operator in_addr const *() const
{
    return this->_data;
}

inline size_t
OddSource::Interfaces::IPv6Address::
data_length() const
{
    return 16;
}

inline
OddSource::Interfaces::IPv6Address::
operator in6_addr const *() const
{
    return this->_data;
}

inline bool
OddSource::Interfaces::IPAddress::
is_unspecified() const
{
    return this->_is_unspecified;
}

inline bool
OddSource::Interfaces::IPAddress::
is_loopback() const
{
    return this->_is_loopback;
}

inline bool
OddSource::Interfaces::IPAddress::
is_link_local() const
{
    return this->_is_link_local;
}

inline bool
OddSource::Interfaces::IPAddress::
is_private() const
{
    return this->_is_private;
}

inline bool
OddSource::Interfaces::IPAddress::
is_multicast() const
{
    return this->_is_multicast;
}

inline bool
OddSource::Interfaces::IPAddress::
is_reserved() const
{
    return this->_is_reserved;
}

inline ::std::optional<OddSource::Interfaces::MulticastScope> const &
OddSource::Interfaces::IPAddress::
multicast_scope() const
{
    return this->_multicast_scope;
}

inline OddSource::Interfaces::IPAddressVersion
OddSource::Interfaces::IPv4Address::
version() const
{
    return IPv4;
}

inline uint8_t
OddSource::Interfaces::IPv4Address::
maximum_prefix_length() const
{
    return 32;
}

inline OddSource::Interfaces::IPAddressVersion
OddSource::Interfaces::IPv6Address::
version() const
{
    return IPv6;
}

inline uint8_t
OddSource::Interfaces::IPv6Address::
maximum_prefix_length() const
{
    return 128;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_unique_local() const
{
    return this->_is_unique_local;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_site_local() const
{
    return this->_is_site_local;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_v4_mapped() const
{
    return this->_is_v4_mapped;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_v4_translated() const
{
    return this->_is_v4_translated;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_v4_compatible() const
{
    return this->_is_v4_compatible;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_6to4() const
{
    return this->_is_6to4;
}

inline ::std::string
OddSource::Interfaces::IPv6Address::
without_scope_id() const
{
    return this->_without_scope;
}

inline ::std::optional<::std::string> const &
OddSource::Interfaces::IPv6Address::
scope_id() const
{
    return this->_scope_id;
}

inline
bool OddSource::Interfaces::IPv6Address::
is_multicast_flag_enabled(OddSource::Interfaces::MulticastV6Flag flag) const
{
    return this->_multicast_flags && (*this->_multicast_flags & flag) == flag;
}

inline
::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, OddSource::Interfaces::IPAddress const & address)
{
    return os << ::std::string(address);
}
