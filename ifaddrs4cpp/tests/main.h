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

#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#define GET_1_OR_2_MACRO(_1,_2,NAME,...) NAME
#define GET_2_OR_3_MACRO(_1,_2,_3,NAME,...) NAME
#define QUOTED_STRINGIFY_ARGUMENT(x) #x
#define ARGUMENT_TO_QUOTED_STRING(x) QUOTED_STRINGIFY_ARGUMENT(x)

namespace OddSource::Interfaces::Tests
{
    template<typename T>
    ::std::string type_id_string(T const &);

    ::std::string demangle(char const * name);

    class TestAssertFailureAbort : public ::std::exception
    {
    public:
        TestAssertFailureAbort() = default;
    };

    class Test
    {
    public:
        Test();

        virtual ~Test() = default;

        virtual void run();

#define add_test(name) this->_tests.emplace(::std::string(ARGUMENT_TO_QUOTED_STRING(name)), [this] { name(); })

        template<typename V1, typename V2>
        inline void assert_equal(V1 const &, V2 const &, ::std::optional<::std::string const>, char const *, int);

#define assert_equal_with_message(v1, v2, message) this->assert_equal(v1, v2, message, __FILE__, __LINE__)
#define assert_equal_without_message(v1, v2) this->assert_equal(v1, v2, ::std::nullopt, __FILE__, __LINE__)
#define assert_equals(...) GET_2_OR_3_MACRO(__VA_ARGS__, assert_equal_with_message, assert_equal_without_message, UNUSED)(__VA_ARGS__)

        template<typename V1, typename V2>
        inline void assert_not_equal(V1 const &, V2 const &, ::std::optional<::std::string const>, char const *, int);

#define assert_not_equal_with_message(v1, v2, message) this->assert_not_equal(v1, v2, message, __FILE__, __LINE__)
#define assert_not_equal_without_message(v1, v2) this->assert_not_equal(v1, v2, ::std::nullopt, __FILE__, __LINE__)
#define assert_not_equals(...) GET_2_OR_3_MACRO(__VA_ARGS__, assert_not_equal_with_message, assert_not_equal_without_message, UNUSED)(__VA_ARGS__)

        inline void assert_true(bool, ::std::optional<::std::string const>, char const *, int);

        inline void assert_true(::std::function<bool()> const &, ::std::optional<::std::string const>, char const *, int);

#define assert_that_with_message(v, message) this->assert_true(v, message, __FILE__, __LINE__)
#define assert_that_without_message(v) this->assert_true(v, ::std::nullopt, __FILE__, __LINE__)
#define assert_that(...) GET_1_OR_2_MACRO(__VA_ARGS__, assert_that_with_message, assert_that_without_message, UNUSED)(__VA_ARGS__)

        inline void assert_false(bool, ::std::optional<::std::string const>, char const *, int);

        inline void assert_false(::std::function<bool()> const &, ::std::optional<::std::string const>, char const *, int);

#define assert_not_that_with_message(v, message) this->assert_false(v, message, __FILE__, __LINE__)
#define assert_not_that_without_message(v) this->assert_false(v, ::std::nullopt, __FILE__, __LINE__)
#define assert_not_that(...) GET_1_OR_2_MACRO(__VA_ARGS__, assert_not_that_with_message, assert_not_that_without_message, UNUSED)(__VA_ARGS__)

        template<class E>
        inline void assert_except(::std::function<void()> const &, ::std::optional<::std::string const>, char const *, int);

#define assert_throws_with_message(p, E, message) this->assert_except<E>([&]() -> void {p;}, message, __FILE__, __LINE__)
#define assert_throws_without_message(p, E) this->assert_except<E>([&]() -> void {p;}, ::std::nullopt, __FILE__, __LINE__)
#define assert_throws(...) GET_2_OR_3_MACRO(__VA_ARGS__, assert_throws_with_message, assert_throws_without_message, UNUSED)(__VA_ARGS__)

#define fail_test(message) this->assert_true(false, message, __FILE__, __LINE__)

        using create_function = ::std::unique_ptr<Test>();

        static void registrate(::std::string const &, create_function *);

        [[nodiscard]]
        static int run_all_registered_test_cases(::std::vector<::std::string> const & matching);

        template<typename T>
        class Registrar
        {
            static_assert(::std::is_base_of_v<Test, T>,
                          "the template parameter T must derive from Test.");

        public:
            explicit Registrar(::std::string const &);

            Registrar(Registrar const &) = delete;

            Registrar(Registrar &&) = delete;

            ~Registrar() = default;
        };

    protected:
        ::std::map<::std::string const, ::std::function<void()> const> _tests;

    private:
        [[nodiscard]]
        static ::std::map<::std::string const, create_function *> & registry();

        inline void failure(::std::string const &, char const *, int);

        inline void error(::std::string const &);

        uint64_t _assertion_count;
        uint16_t _test_count;
        uint16_t _pass_test_count;
        uint16_t _fail_test_count;
        uint16_t _error_test_count;
        ::std::vector<::std::string> _failures;
        ::std::vector<::std::string> _errors;
    };
}

#include "main.hpp"
