try:
    from ifaddrs4py.extern import (
        __VERSION_EXT__,
        __VERSION_INFO_EXT__,
    )
except ImportError:
    __VERSION_EXT__ = "unknown"
    __VERSION_INFO_EXT__ = ()


__VERSION__ = "0.1.0-alpha0"
__VERSION_INFO__ = (0, 1, 0, "alpha0")

__all__ = (
    "__VERSION__",
    "__VERSION_EXT__",
    "__VERSION_INFO__",
    "__VERSION_INFO_EXT__",
)
