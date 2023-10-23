from unittest import TestCase

from ifaddrs4py.interface import (
    InterfaceFlag,
    InterfaceIPAddressFlag,
)


class TestEnums(TestCase):
    def test_interface_ip_address_flags(self):
        for e in InterfaceIPAddressFlag:
            print(f"InterfaceIPAddressFlag.{e.name} = {e.value}")
            self.assertTrue(e.value > 0, f"InterfaceIPAddressFlag.{e.name} is not correct")

    def test_interface_flags(self):
        for e in InterfaceFlag:
            print(f"InterfaceFlag.{e.name} = {e.value}")
            self.assertTrue(e.value > 0, f"InterfaceFlag.{e.name} is not correct")
