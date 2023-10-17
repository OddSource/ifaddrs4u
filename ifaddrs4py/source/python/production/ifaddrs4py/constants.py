import sys

__all__ = (
    "IS_WINDOWS",
)

IS_WINDOWS = sys.platform == "win32" or sys.platform == "win64" or sys.platform == "cygwin"