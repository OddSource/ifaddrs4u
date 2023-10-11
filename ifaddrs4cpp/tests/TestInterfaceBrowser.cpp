#include "../Interfaces.h"
#include "main.h"

using namespace OddSource::Interfaces;

class TestInterfaceBrowser : public Tests::Test
{
public:
    TestInterfaceBrowser()
        : Test(),
          _browser()
    {
        add_test(test_print_all);
        add_test(test_get_by_name);
        add_test(test_get_by_index);
        add_test(test_get_interfaces);
    }

    void test_print_all()
    {
        ::std::function<bool(Interface const &)> do_this = [this](auto interface)
        {
            ::std::cout << interface << ::std::endl;
            assert_that(interface.index() > 0);
            assert_that(interface.name().length() > 0);
#ifdef IS_WINDOWS
            assert_that(interface.windows_uuid().length() > 0);
#endif /* IS_WINDOWS */
            return true;
        };
        ::std::cout << "Interface browser output below: " << ::std::endl;
        this->_browser.for_each_interface(do_this);
    }

    void test_get_by_name()
    {
#ifdef IS_WINDOWS
        // TODO dunno yet
#elif IS_MACOS /* IS_WINDOWS */
        Interface lo(this->_browser.get_interface("lo0"));
        assert_equals(lo.name(), "lo0");
        assert_that(lo.index() > 0);

        Interface en(this->_browser.get_interface("en0"));
        assert_equals(en.name(), "en0");
        assert_that(en.index() > 0);
#else /* !IS_WINDOWS && !IS_MACOS */
        // TODO dunno yet
#endif /* !IS_WINDOWS && !IS_MACOS */

        assert_throws(this->_browser.get_interface("fooBar42"), ::std::invalid_argument);
    }

    void test_get_by_index()
    {
#ifdef IS_WINDOWS
        // TODO dunno yet
#elif IS_MACOS /* IS_WINDOWS */
        Interface if0(this->_browser.get_interface(1));
        assert_not_that(if0.name().empty());
        assert_equals(if0.index(), 1u);

        Interface if1(this->_browser.get_interface(2));
        assert_not_that(if1.name().empty());
        assert_equals(if1.index(), 2u);
#else /* !IS_WINDOWS && !IS_MACOS */
        // TODO dunno yet
#endif /* !IS_WINDOWS && !IS_MACOS */

        assert_throws(this->_browser.get_interface(4'294'967'294), ::std::invalid_argument);
    }

    void test_get_interfaces()
    {
        uint32_t interfaces_found(0);
        bool loopback_found(false);
        bool non_loopback_found(false);
        for (auto const & interface : this->_browser.get_interfaces())
        {
            interfaces_found++;
            if (interface.is_loopback())
            {
                loopback_found = true;
            }
            else
            {
                non_loopback_found = true;
            }
        }
        assert_that(interfaces_found > 1,
                    ::std::string("At least two interfaces should have been found, but ") +
                        ::std::to_string(interfaces_found) + " found instead.");
        assert_that(loopback_found, "At least one loopback interface should have been found.");
        assert_that(non_loopback_found, "At least one non-loopback interface should have been found.");
    }

    static std::unique_ptr<Test> create()
    {
        return std::make_unique<TestInterfaceBrowser>();
    }

private:
    InterfaceBrowser _browser;
};

namespace
{
    [[maybe_unused]]
    Tests::Test::Registrar<TestInterfaceBrowser> registrar("TestInterfaceBrowser");
}
