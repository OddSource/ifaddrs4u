from unittest import TestCase

from ifaddrs4py.constants import IS_WINDOWS
from ifaddrs4py import extern


class TestSamples(TestCase):
    def test_ipv4_loopback(self) -> None:
        self.assertEqual(f"{extern._TEST_LOOPBACK_V4}", "127.0.0.1")

    def test_ipv6_loopback(self) -> None:
        self.assertEqual(f"{extern._TEST_LOOPBACK_V6}", "::1")

    def test_mac_address(self) -> None:
        self.assertEqual(f"{extern._TEST_MAC_ADDRESS}", "ac:de:48:00:11:22")

    def test_get_sample_interface_ipv4_address(self) -> None:
        address = extern.get_sample_interface_ipv4_address()
        self.assertEqual(f"{address.address}", "192.168.0.42")
        self.assertEqual(address.prefix_length, 24)
        self.assertIsNotNone(address.broadcast_address)
        self.assertEqual(f"{address.broadcast_address}", "192.168.0.254")
        self.assertIsNone(address.point_to_point_destination)
        self.assertEqual(f"{address}", "192.168.0.42/24 broadcast 192.168.0.254")

    def test_get_sample_interface_ipv6_address(self) -> None:
        address = extern.get_sample_interface_ipv6_address()
        self.assertEqual(f"{address.address}", "2001:470:2ccb:a61b:e:acf8:6736:d81e")
        self.assertEqual(address.prefix_length, 56)
        self.assertIsNone(address.broadcast_address)
        self.assertIsNone(address.point_to_point_destination)
        self.assertEqual(f"{address}",
                         "2001:470:2ccb:a61b:e:acf8:6736:d81e/56 autoconf secured")

    def test_get_sample_interface_scoped_ipv6_address(self) -> None:
        address = extern.get_sample_interface_scoped_ipv6_address()
        self.assertEqual(f"{address.address}", "fe80::aede:48ff:fe00:1122%en5")
        self.assertEqual(address.prefix_length, 64)
        self.assertIsNone(address.broadcast_address)
        self.assertIsNone(address.point_to_point_destination)
        self.assertEqual(f"{address}", "fe80::aede:48ff:fe00:1122%en5/64 secured scopeid 0x6")

    def test_get_sample_interface(self) -> None:
        interface = extern.get_sample_interface()
        self.assertIsNotNone(interface)
        self.assertEqual(interface.index, 3)
        self.assertEqual(interface.name, "en0")

        if IS_WINDOWS:
            self.assertTrue(hasattr(interface, "windows_uuid"))
            self.assertEqual(interface.windows_uuid, "24af9519-2a42-4f62-99fa-1ed3147ad90a")
        else:
            self.assertFalse(hasattr(interface, "windows_uuid"))

        self.assertEqual(f"{interface.mac_address}", "ac:de:48:00:11:22")

        self.assertEqual(len(interface.ipv4_addresses), 1)
        self.assertEqual("192.168.0.42/24 broadcast 192.168.0.254",
                         f"{interface.ipv4_addresses[0]}")

        self.assertEqual(len(interface.ipv6_addresses), 2)
        self.assertEqual("fe80::aede:48ff:fe00:1122%en5/64 secured scopeid 0x6",
                         f"{interface.ipv6_addresses[0]}")
        self.assertEqual(f"{interface.ipv6_addresses[1]}",
                         "2001:470:2ccb:a61b:e:acf8:6736:d81f/56 autoconf secured")