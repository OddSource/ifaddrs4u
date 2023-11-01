#ifndef WINSOCK_INCLUDES_INCLUDED
#define WINSOCK_INCLUDES_INCLUDED 1
// Windows and Windows socket headers have to be included in a particular order, or else
// all kinds of errors are encountered. Additionally, many (all?) of these header files
// do not have double-inclusion guards. Both of these facts are incredible stupid, but
// we have to account for them. So all Windows includes will be in this file, in the
// correct order, with double-inclusion guards around them.
#include <winsock2.h>
#include <windows.h>
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <ntstatus.h>
#include <ip2string.h>
#endif /* WINSOCK_INCLUDES_INCLUDED */
