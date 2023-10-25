from unittest import TestCase

from ifaddrs4py.mac_address import MacAddress


class TestMacAddress(TestCase):
    def test_construct_with_string(self) -> None:
        address = MacAddress("a4:83:e7:2e:a1:67")

        self.assertEqual(f"{address}", "a4:83:e7:2e:a1:67")
        self.assertEqual(len(address), 6)
        self.assertEqual(address.data_length, 6)
        self.assertEqual(address.data, (0xa4, 0x83, 0xe7, 0x2e, 0xa1, 0x67))

        address = MacAddress("b2:fb:b8:5b:84:e8:fe:ff")

        self.assertEqual(f"{address}", "b2:fb:b8:5b:84:e8:fe:ff")
        self.assertEqual(len(address), 8)
        self.assertEqual(address.data_length, 8)
        self.assertEqual(address.data, (0xb2, 0xfb, 0xb8, 0x5b, 0x84, 0xe8, 0xfe, 0xff))

    def test_construct_with_data(self) -> None:
        address = MacAddress(data=(0x67, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xe3))

        self.assertEqual(f"{address}", "67:a1:2e:ff:a4:e7:e3")
        self.assertEqual(len(address), 7)
        self.assertEqual(address.data_length, 7)
        self.assertEqual(address.data, (0x67, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xe3))

        address = MacAddress(data=(0x67, 0xaa, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xbb))

        self.assertEqual(f"{address}", "67:aa:a1:2e:ff:a4:e7:bb")
        self.assertEqual(len(address), 8)
        self.assertEqual(address.data_length, 8)
        self.assertEqual(address.data, (0x67, 0xaa, 0xa1, 0x2e, 0xff, 0xa4, 0xe7, 0xbb))

    def test_construct_malformed(self) -> None:
        self.assertRaises(ValueError, lambda: MacAddress("67:a1:2e:ff:e5"))
        self.assertRaises(ValueError, lambda: MacAddress("67:a1:2e:ff:e5:e6:e7:e8:e9"))
        self.assertRaises(ValueError, lambda: MacAddress("67a12ef1a4e7"))

        self.assertRaises(ValueError, lambda: MacAddress(data=(0x67, 0xa1, 0x2e, 0xff, 0xa4)))
        self.assertRaises(ValueError, lambda: MacAddress(data=(0x67, 0xa1, 0x2e, 0xff, 0xa4, 0xff, 0xa4, 0xe7, 0xbb)))

    def test_equals(self) -> None:
        self.assertEqual(MacAddress("a4:83:e7:2e:a1:67"), MacAddress("a4:83:e7:2e:a1:67"))
        self.assertEqual(MacAddress("a4:83:e7:2e:a1:67"), MacAddress("A4:83:E7:2E:A1:67"))
        self.assertNotEqual(MacAddress("b2:fb:b8:5b:84:e8"), MacAddress("b2:fb:b8:5b:84:e8:ff"))
        self.assertNotEqual(MacAddress("b2:fb:b8:5b:84:e8:fe"), MacAddress("b2:fb:b8:5b:84:e8:ff"))
