#pragma once

#define CATCH_STD_EXCEPTION_SET_ERROR_IF_NOT_SET(err, ret) catch (::std::exception const & e) \
    { \
        if (PyErr_Occurred() == NULL) \
        { \
            PyErr_SetString(err, e.what()); \
        } \
        ret; \
    }
