/*
 * Copyright © 2010-2023 OddSource Code (license@oddsource.io)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

inline
OddSource::Interfaces::MacAddress::
operator ::std::string() const
{
    return this->_representation;
}

inline
OddSource::Interfaces::MacAddress::
operator char const *() const
{
    return this->_representation.c_str();
}

inline
OddSource::Interfaces::MacAddress::
operator uint8_t const *() const
{
    return this->_data.get();
}

inline uint8_t
OddSource::Interfaces::MacAddress::
length() const
{
    return this->_data_length;
}

inline bool
OddSource::Interfaces::MacAddress::
operator==(MacAddress const & other) const
{
    if (this->_data_length != other._data_length)
    {
        return false;
    }
    for (uint8_t i(0); i < this->_data_length; i++)
    {
        if (this->_data[i] != other._data[i])
        {
            return false;
        }
    }
    return true;
}

inline bool
OddSource::Interfaces::MacAddress::
operator!=(MacAddress const & other) const
{
    return !this->operator==(other);
}

inline ::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, MacAddress const & address)
{
    return os << address.operator::std::string();
}
