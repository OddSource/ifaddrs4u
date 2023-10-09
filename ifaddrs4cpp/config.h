#pragma  once

#include "os.h"
#include "version.h"

#ifdef IS_WINDOWS
#ifdef ODDSOURCE_BUILDING_LIBRARY
#define OddSource_Export __declspec(dllexport)
#else /* ODDSOURCE_BUILDING_LIBRARY */
#define OddSource_Export __declspec(dllimport)
#endif /* ODDSOURCE_BUILDING_LIBRARY */
#else /* IS_WINDOWS */
#define OddSource_Export __attribute((visibility("default")))
#endif /* IS_WINDOWS */

#ifdef IS_WINDOWS
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#endif /* IS_WINDOWS */
