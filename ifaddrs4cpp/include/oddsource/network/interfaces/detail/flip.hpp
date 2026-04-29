#pragma once

#include <unordered_map>

namespace OddSource::Interfaces::detail
{
    template< class K, class V >
    std::unordered_map< V, K const >
    flip(
        ::std::unordered_map< K, V const > const & source )
    {
        ::std::unordered_map< V, K const > destination;
        destination.reserve( source.size() );
        for ( auto const & [ key, value ] : source )
        {
            destination.emplace( value, key );
        }
        return destination;
    }
}
