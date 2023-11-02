#ifndef S_H_INCLUDED
#define S_H_INCLUDED 1

#include <string>

#endif /* S_H_INCLUDED */

#ifndef IS_WINDOWS
#pragma GCC diagnostic push
#ifdef __clang__
#pragma GCC diagnostic ignored "-Wreserved-user-defined-literal"
#elif !defined( __has_warning ) || __has_warning("-Wliteral-suffix")
#pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif
#endif /* IS_WINDOWS */
using std::string_literals::operator""s;
#ifndef IS_WINDOWS
#pragma GCC diagnostic pop
#endif /* IS_WINDOWS */
