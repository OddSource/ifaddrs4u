from unittest import TestCase

from ifaddrs4py import version


class TestVersionInfo(TestCase):
    def test_version_info(self):
        self.assertEqual(version.__VERSION__, version.__VERSION_EXT__)
        self.assertEqual(version.__VERSION_INFO__, version.__VERSION_INFO_EXT__)
