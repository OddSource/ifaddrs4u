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

    ::std::string repr_str(repr);
    auto data = new Addr[sizeof(Addr)];
    int success;
    if constexpr (::std::is_same_v<Addr, in6_addr>)
    {
        // inet_pton can also handle IPv4 addresses, but only in dotted-decimal format
        // (1.2.3.4), not in octal, hexadecimal, or any other valid IPv4 format.
        success = inet_pton(AF_INET6, repr_str.c_str(), data);
    }
    else
    {
        int num_dots(0);
        for (char c : repr)
        {
            if (c == '.')
            {
                num_dots++;
            }
        }
        if (num_dots != 3)
        {
            // some implementations of inet_aton tolerate incomplete addresses, but we do not
            throw InvalidIPAddress(
                    "Malformed IPv4 address string '"s + repr_str + "' with "s +
                    ::std::to_string(num_dots + 1) + " parts instead of 4"s);
        }
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
        success = inet_aton(repr_str.c_str(), data);
#endif
    }
    if (success != 1)
    {
        delete[] data;
        throw InvalidIPAddress("Malformed IP address string '"s + repr_str + "' or unknown inet_*ton error."s);
    }

    return data;
}

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
    auto ptr = ::inet_ntop(family, data, host_chars, host_length);
    if (ptr == nullptr)
    {
#ifdef IS_WINDOWS
        auto err_no(::WSAGetLastError());
        wchar_t * s = nullptr;
        ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr, err_no,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPWSTR)&s, 0, nullptr);
        ::std::string err(
            s == nullptr ?
            ""s :
            ::std::wstring_convert<::std::codecvt_utf8<wchar_t>, wchar_t>().to_bytes(::std::wstring(s)));
        LocalFree(s);
#else /* IS_WINDOWS */
        auto err_no(errno);
        char const * err(
            err_no == EAFNOSUPPORT ? "Address family not supported" :
                (err_no == ENOSPC ? "Converted address would exceed string size" :
                    ::gai_strerror(errno)));
#endif /* IS_WINDOWS */
        throw InvalidIPAddress(
            (::std::ostringstream() << "Malformed in_addr data or inet_ntop system error: "
                                    << err_no << " (" << err << ")").str()
        );
    }
    return host_chars;
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

inline bool
OddSource::Interfaces::IPv4Address::
operator==(OddSource::Interfaces::IPv4Address const & other) const
{
    return (*reinterpret_cast<uint32_t const *>(this->_data) ==
            *reinterpret_cast<uint32_t const *>(other._data));
}

inline bool
OddSource::Interfaces::IPv4Address::
operator!=(OddSource::Interfaces::IPv4Address const & other) const
{
    return !this->operator==(other);
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

inline bool
OddSource::Interfaces::IPv6Address::
has_scope_id() const
{
    return (bool)this->_scope;
}

inline ::std::string
OddSource::Interfaces::IPv6Address::
without_scope_id() const
{
    return this->_without_scope;
}

inline ::std::optional<uint32_t> const &
OddSource::Interfaces::IPv6Address::
scope_id() const
{
    static ::std::optional<uint32_t> const nil; // prevent "returning ref to temp local"
    return this->_scope ? this->_scope->scope_id : nil;
}

inline ::std::optional<::std::string> const &
OddSource::Interfaces::IPv6Address::
scope_name() const
{
    static ::std::optional<::std::string> const nil; // prevent "returning ref to temp local"
    return this->_scope ? this->_scope->scope_name : nil;
}

inline ::std::optional<::std::string>
OddSource::Interfaces::IPv6Address::
scope_name_or_id() const
{
    if (!this->_scope)
    {
        return ::std::nullopt;
    }
    return this->_scope->scope_name ? this->_scope->scope_name : ::std::to_string(*this->_scope->scope_id);
}

inline ::std::optional<::std::string>
OddSource::Interfaces::IPv6Address::
scope_id_or_name() const
{
    if (!this->_scope)
    {
        return ::std::nullopt;
    }
    return this->_scope->scope_id ? ::std::to_string(*this->_scope->scope_id) : this->_scope->scope_name;
}

inline bool
OddSource::Interfaces::IPv6Address::
is_multicast_flag_enabled(OddSource::Interfaces::MulticastV6Flag flag) const
{
    return this->_multicast_flags && (*this->_multicast_flags & flag) == flag;
}

inline bool
OddSource::Interfaces::IPv6Address::
operator==(OddSource::Interfaces::IPv6Address const & other) const
{
    auto data1 = reinterpret_cast<uint8_t const *>(this->_data);
    auto data2 = reinterpret_cast<uint8_t const *>(other._data);
    auto length = this->data_length();
    for(size_t i(0); i < length; i++)
    {
        if (data1[i] != data2[i])
        {
            return false;
        }
    }
    return true;
}

inline bool
OddSource::Interfaces::IPv6Address::
operator!=(OddSource::Interfaces::IPv6Address const & other) const
{
    return !this->operator==(other);
}

inline ::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, OddSource::Interfaces::IPAddress const & address)
{
    return os << ::std::string(address);
}
