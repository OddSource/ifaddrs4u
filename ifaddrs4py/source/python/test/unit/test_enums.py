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

from ifaddrs4py.interface import (
    InterfaceFlag,
    InterfaceIPAddressFlag,
)


class TestEnums(TestCase):
    def test_interface_ip_address_flags(self) -> None:
        for e in InterfaceIPAddressFlag:
            print(f"InterfaceIPAddressFlag.{e.name} = {e.value}")
            self.assertTrue(e.value > 0, f"InterfaceIPAddressFlag.{e.name} is not correct")

    def test_interface_flags(self) -> None:
        for e in InterfaceFlag:
            print(f"InterfaceFlag.{e.name} = {e.value}")
            self.assertTrue(e.value > 0, f"InterfaceFlag.{e.name} is not correct")
