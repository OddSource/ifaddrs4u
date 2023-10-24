import sys

__all__ = (
    "IS_MACOS",
    "IS_WINDOWS",
)

IS_MACOS = sys.platform == "darwin" or sys.platform == "macos"
IS_WINDOWS = sys.platform == "win32" or sys.platform == "win64" or sys.platform == "cygwin"
