from unittest import TestCase

from ifaddrs4py.browser import InterfaceBrowser
from ifaddrs4py.constants import IS_MACOS
from ifaddrs4py.interface import (
    Interface,
    InterfaceFlag,
)


class TestInterfaceBrowser(TestCase):
    def test_for_each_then_iterate(self) -> None:
        counts = [0, 0]

        def do_this(i: Interface) -> bool:
            print(f"{i}")
            counts[0] += 1
            return True

        browser = InterfaceBrowser()
        self.assertTrue(browser.for_each_interface(do_this))

        for interface in browser:
            counts[1] += 1
            self.assertIsNotNone(interface)

        self.assertEqual(counts[0], counts[1])
        self.assertTrue(counts[0] > 0)
        self.assertTrue(counts[1] > 0)

    def test_iterate_then_foreach(self) -> None:
        counts = [0, 0]

        def do_this(_: Interface) -> bool:
            counts[1] += 1
            return True

        browser = InterfaceBrowser()

        lens = [0, 0, 0, 0]

        lens[0] = len(browser)
        lens[1] = browser.length()

        for interface in browser:
            counts[0] += 1
            self.assertIsNotNone(interface)

        self.assertTrue(browser.for_each_interface(do_this))

        self.assertEqual(counts[0], counts[1])
        self.assertTrue(counts[0] > 0)
        self.assertTrue(counts[1] > 0)

        lens[2] = len(browser)
        lens[3] = browser.length()

        self.assertEqual(counts[0], lens[0])
        self.assertEqual(counts[0], lens[1])
        self.assertEqual(counts[0], lens[2])
        self.assertEqual(counts[0], lens[3])

    def test_iterate_then_iterate(self) -> None:
        counts = [0, 0]

        def do_this(_: Interface) -> bool:
            counts[0] += 1
            return True

        def do_that(_: Interface) -> bool:
            counts[1] += 1
            return True

        browser = InterfaceBrowser()
        self.assertTrue(browser.for_each_interface(do_this))
        self.assertTrue(browser.for_each_interface(do_that))

        self.assertTrue(counts[0] > 0)
        self.assertEqual(counts[0], counts[1])

    def test_for_each_with_returning_false(self) -> None:
        counts = [0, 0]

        def do_this(_: Interface) -> bool:
            counts[0] += 1
            return False

        def do_that(_: Interface) -> bool:
            counts[1] += 1
            return False if counts[1] > 1 else True

        browser = InterfaceBrowser()
        self.assertFalse(browser.for_each_interface(do_this))
        self.assertFalse(browser.for_each_interface(do_that))

        self.assertEqual(counts[0], 1)
        self.assertEqual(counts[1], 2)

    def test_get_by_name(self) -> None:
        browser = InterfaceBrowser()

        if IS_MACOS:
            lo = browser.get_interface("lo0")
            self.assertIsNotNone(lo)
            assert lo is not None  # for mypy
            self.assertEqual(lo.name, "lo0")
            self.assertTrue(lo.index > 0)

            lo = browser["lo0"]
            self.assertEqual(lo.name, "lo0")
            self.assertTrue(lo.index > 0)

            en = browser.get_interface("en0")
            self.assertIsNotNone(en)
            assert en is not None  # for mypy
            self.assertEqual(en.name, "en0")
            self.assertTrue(en.index > 0)

            en = browser["en0"]
            self.assertEqual(en.name, "en0")
            self.assertTrue(en.index > 0)

            self.assertIsNone(browser.get_interface("fooBar42"))
            self.assertRaises(KeyError, lambda: browser["fooBar42"])

    def test_get_by_index(self) -> None:
        browser = InterfaceBrowser()

        if IS_MACOS:
            if1 = browser.get_interface(1)
            self.assertIsNotNone(if1)
            assert if1 is not None  # for mypy
            self.assertEqual(if1.index, 1)
            self.assertIsNotNone(if1.name)

            if1 = browser[1]
            self.assertEqual(if1.index, 1)
            self.assertIsNotNone(if1.name)

            if2 = browser.get_interface(2)
            self.assertIsNotNone(if2)
            assert if2 is not None  # for mypy
            self.assertEqual(if2.index, 2)
            self.assertIsNotNone(if2.name)

            if2 = browser[2]
            self.assertEqual(if2.index, 2)
            self.assertIsNotNone(if2.name)

            self.assertIsNone(browser.get_interface(2_147_483_645))
            self.assertRaises(KeyError, lambda: browser[2_147_483_645])

    def test_get_interfaces(self) -> None:
        interfaces_found = 0
        loopback_found = False
        non_loopback_found = False

        browser = InterfaceBrowser()
        for interface in browser.get_interfaces():
            interfaces_found += 1
            if interface.is_flag_enabled(InterfaceFlag.IsLoopback):
                loopback_found = True
            else:
                non_loopback_found = True

        self.assertTrue(interfaces_found > 1)
        self.assertTrue(loopback_found)
        self.assertTrue(non_loopback_found)

    def test_no_segfault(self) -> None:
        counts = [0, 0, 0]

        for interface in InterfaceBrowser().get_interfaces():
            counts[0] += 1

        for interface in InterfaceBrowser():
            counts[1] += 1

        def do_this(_: Interface) -> bool:
            counts[2] += 1
            return True

        self.assertTrue(InterfaceBrowser().for_each_interface(do_this))

        self.assertTrue(counts[0] > 0)
        self.assertTrue(counts[1] > 0)
        self.assertTrue(counts[2] > 0)
        self.assertEqual(counts[0], counts[1])
        self.assertEqual(counts[1], counts[2])
