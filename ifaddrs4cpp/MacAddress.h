#pragma once

#include "config.h"

#include <iostream>
#include <string>
#include <string_view>

#ifndef MAX_ADAPTER_ADDRESS_LENGTH
#define MAX_ADAPTER_ADDRESS_LENGTH 8
#endif /* MAX_ADAPTER_ADDRESS_LENGTH */

#define ADAPTER_ADDRESS_FROM_REPR_LENGTH 6

namespace OddSource::Interfaces
{
    class OddSource_Export InvalidMacAddress : public ::std::invalid_argument
    {
    public:
        explicit InvalidMacAddress(::std::string_view const & what)
                : ::std::invalid_argument(::std::string(what)) {}
        InvalidMacAddress(InvalidMacAddress const & other) noexcept = default;
    };

    class OddSource_Export MacAddress
    {
    public:
        MacAddress() = delete;

        // copy constructor
        MacAddress(MacAddress const &);

        // move constructor
        MacAddress(MacAddress &&) noexcept;

        MacAddress(::std::string_view const &); // NOLINT(*-explicit-constructor)

        MacAddress(uint8_t const [MAX_ADAPTER_ADDRESS_LENGTH], uint8_t length);

        ~MacAddress();

        inline operator ::std::string() const; // NOLINT(*-explicit-constructor)

        inline operator char const *() const; // NOLINT(*-explicit-constructor)

        inline operator uint8_t const *() const; // NOLINT(*-explicit-constructor)

        inline uint8_t length() const;

    private:
        MacAddress(::std::string_view const &, uint8_t const *, uint8_t);

        static uint8_t const * from_repr(::std::string_view const &);

        static ::std::string to_repr(uint8_t const [MAX_ADAPTER_ADDRESS_LENGTH], uint8_t);

        ::std::string const _representation;
        uint8_t const * _data;
        uint8_t const _data_length;
    };

    inline ::std::ostream & operator<<(::std::ostream &, MacAddress const &);
}

#include "MacAddress.hpp"
