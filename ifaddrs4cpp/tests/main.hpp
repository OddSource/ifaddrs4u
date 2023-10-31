/*
 * Copyright © 2010-2023 OddSource Code (license@oddsource.io)
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
            message.emplace("The boolean condition was unexpectedly false");
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
            message.emplace("The predicate unexpectedly returned false");
        }
        this->failure(*message, file, line);
        throw TestAssertFailureAbort();
    }
}

inline void
OddSource::Interfaces::Tests::Test::
assert_false(bool test, ::std::optional<::std::string const> message, char const * file, int line)
{
    this->_assertion_count++;
    if (test)
    {
        if (!message)
        {
            message.emplace("The boolean condition was unexpectedly true");
        }
        this->failure(*message, file, line);
        throw TestAssertFailureAbort();
    }
}

inline void
OddSource::Interfaces::Tests::Test::
assert_false(
    ::std::function<bool()> const & test,
    ::std::optional<::std::string const> message,
    char const * file, int line)
{
    this->_assertion_count++;
    if (test())
    {
        if (!message)
        {
            message.emplace("The predicate unexpectedly returned true");
        }
        this->failure(*message, file, line);
        throw TestAssertFailureAbort();
    }
}

template<class E>
inline void
OddSource::Interfaces::Tests::Test::
assert_except(
    ::std::function<void()> const & predicate,
    ::std::optional<::std::string const> message,
    char const * file,
    int line)
{
    try
    {
        predicate();
        if (!message)
        {
            message.emplace((::std::ostringstream() << "Expected exception of type "
                                                    << demangle(typeid(E).name())
                                                    << ", but no exception thrown.").str());
        }
        this->failure(*message, file, line);
    }
    catch(E const &)
    {
        // expected
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
