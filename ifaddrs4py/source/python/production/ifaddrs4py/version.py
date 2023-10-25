try:
    from ifaddrs4py.extern import (
        __VERSION_EXT__,
        __VERSION_INFO_EXT__,
    )
    _skip_version_check = False
except ImportError:
    __VERSION_EXT__ = "unknown"
    __VERSION_INFO_EXT__ = ()
    _skip_version_check = True

__VERSION__ = "0.1.0-alpha0"
__VERSION_INFO__ = (0, 1, 0, "alpha0")

__all__ = (
    "__VERSION__",
    "__VERSION_EXT__",
    "__VERSION_INFO__",
    "__VERSION_INFO_EXT__",
)

if not _skip_version_check:
    import warnings

    class VersionMismatchWarning(UserWarning):
        pass

    if __VERSION__ != __VERSION_EXT__:
        warnings.warn(
            f"ifaddrs4py version: C++ extension version {__VERSION_EXT__} "
            f"does not match Python library version {__VERSION__}",
            category=VersionMismatchWarning,
            stacklevel=1,
        )
    elif __VERSION_INFO__ != __VERSION_INFO_EXT__:
        warnings.warn(
            f"ifaddrs4py version: C++ extension version info {__VERSION_INFO__} "
            f"does not match Python library version info {__VERSION_INFO_EXT__}",
            category=VersionMismatchWarning,
            stacklevel=1,
        )
