#ifndef WINSOCK_INCLUDES_INCLUDED
#define WINSOCK_INCLUDES_INCLUDED 1
// Windows and Windows socket headers have to be included in a particular order, or else
// all kinds of errors are encountered. Additionally, many (all?) of these header files
// do not have double-inclusion guards. And it gets even better! nsstatus.h/winternl.h are
// required to get STATUS_SUCCESS and NTSTATUS properly defined, but windows.h defines a
// subset of the same macros, and there are no checks for to prevent macro redefinition.
// All of these facts are incredible stupid and indicate the Windows programmers don't know
// what they're doing, but we have to account for them. So all Windows includes will be in
// this file, in the correct order, with double-inclusion guards around them, and with
// the windows.h include surrounded by WIN32_NO_STATUS to prevent macro redefinition.
#include <winsock2.h>
#define WIN32_NO_STATUS
#include <windows.h>
#undef WIN32_NO_STATUS
#include <ws2ipdef.h>
#include <iphlpapi.h>
#include <ntstatus.h>
#include <winternl.h>
#include <ip2string.h>
#endif /* WINSOCK_INCLUDES_INCLUDED */
