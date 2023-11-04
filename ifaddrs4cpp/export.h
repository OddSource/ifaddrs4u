#ifdef IS_WINDOWS
#ifdef ODDSOURCE_BUILDING_LIBRARY
#pragma message("COMPILATION DEBUG: Defining OddSource_Export as __declspec(dllexport)")
#define OddSource_Export __declspec(dllexport)
#else /* ODDSOURCE_BUILDING_LIBRARY */
#pragma message("COMPILATION DEBUG: Defining OddSource_Export as __declspec(dllimport)")
#define OddSource_Export __declspec(dllimport)
#endif /* !ODDSOURCE_BUILDING_LIBRARY */
#else /* IS_WINDOWS */
#define OddSource_Export __attribute((visibility("default")))
#endif /* IS_WINDOWS */
