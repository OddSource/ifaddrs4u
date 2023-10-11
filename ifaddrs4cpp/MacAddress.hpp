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
    return this->_data;
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
    return os << ::std::string(address);
}
