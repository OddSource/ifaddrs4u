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

inline
::std::ostream &
OddSource::Interfaces::
operator<<(::std::ostream & os, MacAddress const & address)
{
    return os << ::std::string(address);
}
