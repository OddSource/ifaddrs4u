#
# Copyright © 2010-2023 OddSource Code (license@oddsource.io)
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

from unittest import TestCase

from ifaddrs4py.ip_address import IPv6Address


class TestIPv6Address(TestCase):
    def test_no_scope(self) -> None:
        address = IPv6Address("::1")
        self.assertEqual(f"{address}", "::1")
        self.assertIsNone(address.scope_id)
        self.assertIsNone(address.scope_number)

    def test_string_scope_in_address(self) -> None:
        address = IPv6Address("fe80::aede:48ff:fe00:1122%en5")
        self.assertEqual(f"{address}", "fe80::aede:48ff:fe00:1122%en5")
        self.assertEqual("en5", address.scope_id)
        self.assertIsNone(address.scope_number)

    def test_int_scope_in_adress(self) -> None:
        address = IPv6Address("fe80::aede:48ff:fe00:1122%4")
        self.assertEqual(f"{address}", "fe80::aede:48ff:fe00:1122%4")
        self.assertEqual("4", address.scope_id)
        self.assertEqual(4, address.scope_number)

    def test_string_scope_id_only_with_bytes(self) -> None:
        address = IPv6Address(
            b"\x20\x01\x00\x00\x00\x00\x00\x00\x00\x00\xfe\xac\x71\xbb\x00\x01",
            scope_id="eth0",
        )
        self.assertEqual(f"{address}", "2001::feac:71bb:1%eth0")
        self.assertEqual("eth0", address.scope_id)
        self.assertIsNone(address.scope_number)

    def test_numeric_scope_id_only_with_bytes(self) -> None:
        address = IPv6Address(
            b"\x20\x01\x00\x00\x00\x00\x00\x00\x00\x00\xfe\xac\x71\xbb\x00\x01",
            scope_id="2",
        )
        self.assertEqual(f"{address}", "2001::feac:71bb:1%2")
        self.assertEqual("2", address.scope_id)
        self.assertEqual(2, address.scope_number)

    def test_scope_number_only_with_bytes(self) -> None:
        address = IPv6Address(
            b"\x20\x01\x00\x00\x00\x00\x00\x00\x00\x00\xfe\xac\x71\xbb\x00\x01",
            scope_number=7,
        )
        self.assertEqual(f"{address}", "2001::feac:71bb:1%7")
        self.assertEqual("7", address.scope_id)
        self.assertEqual(7, address.scope_number)

    def test_scope_id_and_number_with_bytes(self) -> None:
        address = IPv6Address(
            b"\x20\x01\x00\x00\x00\x00\x00\x00\x00\x00\xfe\xac\x71\xbb\x00\x01",
            scope_id="lo",
            scope_number=1,
        )
        self.assertEqual(f"{address}", "2001::feac:71bb:1%lo")
        self.assertEqual("lo", address.scope_id)
        self.assertEqual(1, address.scope_number)
