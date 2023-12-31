#ifndef CONSTEXPR_MAP_HPP
#define CONSTEXPR_MAP_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <stdexcept>

/**
 * \namespace fleropp:util
 */
namespace fleropp::util
{
    /**
     * \class ConstexprMap
     * \brief A class that represents a compile-time map
     *
     * This is an extremely simple implementation of a `constexpr` map, represented
     * by a `std::array` of `std::pair`.
     *
     * \tparam K The key type.
     * \tparam V The value type.
     * \tparam size The size of the map.
     */
    template <typename K, typename V, std::size_t size>
    class ConstexprMap {
    public:
        /**
         * \brief The data which contains the map
         */
        std::array<std::pair<K, V>, size> data;
        /**
         * \brief Retrieves the value associated with the given key
         * \param[in] key The key for which the value will be retrieved
         * \return The value associated with the given key
         */
        constexpr V operator[](const K &key) const {
            const auto element = std::find_if(std::begin(data), std::end(data),
                                              [&key](const auto &value) {
                                                  return value.first == key;
                                              });
            if (element != std::end(data)) {
                return element->second;
            }
            else {
                throw std::range_error("Not found.");
            }
        }
    };
}

#endif /* CONSTEXPR_MAP_HPP */