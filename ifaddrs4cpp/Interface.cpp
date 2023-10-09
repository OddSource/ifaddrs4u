#include "Interface.h"

OddSource::Interfaces::Interface::
Interface(
    uint32_t & index,
    ::std::string_view const & name,
#ifdef IS_WINDOWS
    ::std::string_view const & windows_uuid,
#endif /* IS_WINDOWS */
    uint32_t flags)
    : _index(index),
      _name(name),
#ifdef IS_WINDOWS
      _windows_uuid(windows_uuid),
#endif /* IS_WINDOWS */
      _flags(flags),
      _mac_address(),
      _ipv4_addresses(),
      _ipv6_addresses()
{
}
