/**********************************************************************
 * @brief Check if tuple contains type at compile-time
 * @details Based heavily off BitsOfQ's Youtube tutorials.
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#include <tuple>
#include <type_traits>

namespace utils {

/**
 * @brief Check if tuple contains type at compile-time. Based heavily off
 * BitsOfQ's Youtube tutorials.
 * @tparam Type Type to check for
 * @tparam Tuple Tuple to check
 * @tparam CurrIndex Index to start checking from (implementation detail)
 * @example
 * std::tuple<int, float, double> t;
 * tuple_contains_type<int, decltype(t)>::value; // true
 * tuple_contains_type<char, decltype(t)>::value; // false
 */
template <typename Type, typename Tuple, std::size_t CurrIndex = 0>
struct tuple_contains_type
    : std::conditional<

          // Does type at current index equal Type?
          std::is_same_v<std::tuple_element_t<CurrIndex, Tuple>, Type>,

          // If so, return true
          std::true_type,

          // Else, recurse to next index if not at end of tuple
          typename std::conditional<
              CurrIndex + 1 == std::tuple_size_v<Tuple>, std::false_type,
              typename tuple_contains_type<Type, Tuple, CurrIndex + 1>::type>::
              type>::type {};

/**
 * Empty tuple edge-case
 */
template <typename Type>
struct tuple_contains_type<Type, std::tuple<>, 0> : std::false_type {};

}
