#include <iostream>
#include <optional>
#include <sstream>

template<typename T>
::std::string
OddSource::Interfaces::Tests::
type_id_string(T const & t)
{
    return demangle(typeid(t).name());
}

template<typename T>
OddSource::Interfaces::Tests::Test::Registrar<T>::
Registrar(::std::string const & name)
{
    OddSource::Interfaces::Tests::Test::registrate(name, &T::create);
}

template<typename V1, typename V2>
inline void
OddSource::Interfaces::Tests::Test::
assert_equal(
    V1 const & v1, V2 const & v2,
    ::std::optional<::std::string const> message,
    char const * file, int line)
{
    this->_assertion_count++;
    if (v1 != v2)
    {
        ::std::ostringstream oss;
        oss << "v1 [" << type_id_string(v1) << "(" << v1 << ")] != v2 ["
                      << type_id_string(v2) << "(" << v2 << ")], but they should be equal.";

        ::std::string msg(oss.str());
        if (message)
        {
            msg += " " + *message;
        }

        this->failure(msg, file, line);
        throw TestAssertFailureAbort();
    }
}

template<typename V1, typename V2>
inline void
OddSource::Interfaces::Tests::Test::
assert_not_equal(
    V1 const & v1, V2 const & v2,
    ::std::optional<::std::string const> message,
    char const * file, int line)
{
    this->_assertion_count++;
    if (v1 == v2)
    {
        ::std::ostringstream oss;
        oss << "v1 [" << type_id_string(v1) << "(" << v1 << ")] == v2 ["
                      << type_id_string(v2) << "(" << v2 << ")], but they should not be equal";

        ::std::string msg(oss.str());
        if (message)
        {
            msg += " " + *message;
        }

        this->failure(msg, file, line);
        throw TestAssertFailureAbort();
    }
}

inline void
OddSource::Interfaces::Tests::Test::
assert_true(bool test, ::std::optional<::std::string const> message, char const * file, int line)
{
    this->_assertion_count++;
    if (!test)
    {
        if (!message)
        {
            message.emplace("The boolean condition was false");
        }
        this->failure(*message, file, line);
        throw TestAssertFailureAbort();
    }
}

inline void
OddSource::Interfaces::Tests::Test::
assert_true(
    ::std::function<bool()> const & test,
    ::std::optional<::std::string const> message,
    char const * file, int line)
{
    this->_assertion_count++;
    if (!test())
    {
        if (!message)
        {
            message.emplace("The predicate returned false");
        }
        this->failure(*message, file, line);
        throw TestAssertFailureAbort();
    }
}

inline void
OddSource::Interfaces::Tests::Test::
failure(::std::string const & message, char const * file, int line)
{
    this->_failures.emplace_back((
        ::std::ostringstream() << file << ":" << line << " - " << message).str());
}

inline void
OddSource::Interfaces::Tests::Test::
error(::std::string const & message)
{
    this->_errors.emplace_back(message);
}
