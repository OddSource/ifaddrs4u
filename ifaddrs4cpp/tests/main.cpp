#include "main.h"

#include <chrono>
#include <cmath>
#include <cstring>
#include <cstdio>
#include <unistd.h>

#if (defined(__GNUC__) && __GNUC__ >= 3) || defined(__clang__)
#include <cstdlib>
#include <memory>
#include <cxxabi.h>

namespace
{
    struct handle
    {
        char *p;

        explicit handle(char *ptr)
                : p(ptr)
        {
        }

        ~handle()
        {
            std::free(p);
        }
    };
}

::std::string
OddSource::Interfaces::Tests::
demangle(char const * name)
{
    int status = -4; // some arbitrary value to eliminate the compiler warning
    handle result(abi::__cxa_demangle(name, nullptr, nullptr, &status));
    return (status == 0) ? result.p : name;
}
#else
::std::string
OddSource::Interfaces::Tests::
demangle(char const * name)
{
    return name;
}
#endif

OddSource::Interfaces::Tests::Test::
Test()
    : _tests(),
      _test_count(0),
      _failures(),
      _errors()
{
    this->_failures.reserve(50);
    this->_errors.reserve(50);
}

void
OddSource::Interfaces::Tests::Test::
registrate(::std::string const & name, create_function * function)
{
    OddSource::Interfaces::Tests::Test::registry()[name] = function;
}

::std::map<::std::string const, OddSource::Interfaces::Tests::Test::create_function *> &
OddSource::Interfaces::Tests::Test::
registry()
{
    static ::std::map<::std::string const, OddSource::Interfaces::Tests::Test::create_function *> impl;
    return impl;
}

void
OddSource::Interfaces::Tests::Test::
run()
{
    bool const is_tty(::isatty(::fileno(::stdin)));
    char const * RED = is_tty ? "\033[0;31m" : "";
    char const * ORANGE = is_tty ? "\033[0;33m" : "";
    char const * RESET = is_tty ? "\033[0m" : "";
    char const * ERROR = is_tty ? "\033[0;31mERROR\033[0m" : "ERROR";
    char const * FAIL = is_tty ? "\033[0;33mFAIL\033[0m" : "FAIL";
    char const * PASS = is_tty ? "\033[0;32mPASS\033[0m" : "PASS";

    for (auto const & [name, test_function] : this->_tests)
    {
        ::std::cout << "  ::" << name << " ... " << ::std::flush;
        size_t failure_count(this->_failures.size());
        size_t error_count(this->_errors.size());
        this->_test_count++;
        try
        {
            test_function();
        }
        catch (TestAssertFailureAbort const &)
        {
            // do nothing, this was just to cause test_function() to return
        }
        catch (::std::exception const & e)
        {
            this->error((
                ::std::ostringstream() << "Exception '" << type_id_string(e)
                                       << "' occurred: " << e.what()).str());
        }
        ::std::cout << (error_count != this->_errors.size() ? ERROR :
                           (failure_count != this->_failures.size() ? FAIL : PASS))
                    << ::std::endl;
        for(; error_count < this->_errors.size(); error_count++)
        {
            ::std::cerr << RED << "    ERROR: " << this->_errors[error_count] << RESET << ::std::endl;
        }
        for(; failure_count < this->_failures.size(); failure_count++)
        {
            ::std::cerr << ORANGE << "    ASSERTION: " << this->_failures[failure_count] << RESET << ::std::endl;
        }
    }
}

int
OddSource::Interfaces::Tests::Test::
run_all_registered_test_cases(::std::vector<::std::string const> const & matching)
{
    uint32_t total_test_count(0);
    uint64_t total_assertion_count(0);
    ::std::chrono::steady_clock::time_point start_time(::std::chrono::steady_clock::now());
    auto end = matching.end();
    int ret = 0;
    for (auto const & [name, create_function] : OddSource::Interfaces::Tests::Test::registry())
    {
        if (!matching.empty() && ::std::find(matching.begin(), end, name) == end)
        {
            continue;
        }
        ::std::cout << "Running test case " << name << "..." << ::std::endl;
        auto test = create_function();
        test->run();
        if (!test->_failures.empty() || !test->_errors.empty())
        {
            ::std::cerr << "  Case failed with " << test->_failures.size() << " assertion failures and "
                        << test->_errors.size() << " errors." << ::std::endl;
            ret = 1;
        }
        if (test->_test_count != test->_tests.size())
        {
            ::std::cerr << "  NOTE: Not all tests in test case ran." << std::endl;
            ret = 1;
        }
        total_test_count += test->_test_count;
        total_assertion_count += test->_assertion_count;
    }

    ::std::chrono::steady_clock::time_point end_time(std::chrono::steady_clock::now());
    long double elapsed_microseconds(std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count());
    ::std::ostringstream oss;
    oss.precision(5);
    if (elapsed_microseconds > 1'000'000)
    {
        oss.precision(9);
        auto elapsed_seconds(elapsed_microseconds / 1'000'000.0);
        oss << elapsed_seconds << " seconds";
    }
    else if (elapsed_microseconds > 1'000)
    {
        auto elapsed_milliseconds(elapsed_microseconds / 1'000.0);
        oss << elapsed_milliseconds << " milliseconds";
    }
    else
    {
        oss << elapsed_microseconds << " microseconds";
    }
    ::std::string elapsed(oss.str());
    ::std::cout << "Ran " << total_test_count << " tests (" << total_assertion_count
                << " assertions) in " << elapsed << "." << ::std::endl;

    return ret;
}

int main(int argc, char * argv [])
{
    ::std::vector<::std::string const> matching;
    matching.reserve(argc - 1);
    for (int i = 1; i < argc; i++)
    {
        if (::std::strlen(argv[i]) > 0)
        {
            matching.emplace_back(argv[i]);
        }
    }
    return OddSource::Interfaces::Tests::Test::run_all_registered_test_cases(matching);
}
