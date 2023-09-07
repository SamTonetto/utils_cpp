#/**********************************************************************
  * @brief Check if container is an STL container
  * @details 
  * @author Sam Tonetto
  * @copyright GNU Public License
  * @date 2023
  ***********************************************************************/

#pragma once

#include "metaprogramming/is_instantiation.hpp"

#include <bitset>
#include <forward_list>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <span>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <variant>
#include <vector>

namespace utils {

template <typename T>
constexpr bool is_stl_container() {

  // or-statement roughly ordered by popularity.

  return is_instantiation<std::vector, T>() ||
         is_instantiation<std::map, T>() ||
         is_instantiation<std::unordered_map, T>() ||
         is_instantiation<std::set, T>() ||
         is_instantiation<std::unordered_set, T>() ||
         is_instantiation<std::pair, T>() ||
         is_instantiation<std::tuple, T>() ||
         is_instantiation<std::array, T>() ||
         is_instantiation<std::stack, T>() ||
         is_instantiation<std::queue, T>() ||
         is_instantiation<std::priority_queue, T>() ||
         is_instantiation<std::deque, T>() ||
         is_instantiation<std::variant, T>() ||
         is_instantiation<std::list, T>() || is_instantiation<std::span, T>() ||
         is_instantiation<std::bitset, T>() ||
         is_instantiation<std::valarray, T>() ||
         is_instantiation<std::forward_list, T>() ||
         is_instantiation<std::multiset, T>() ||
         is_instantiation<std::multimap, T>() ||
         is_instantiation<std::unordered_multiset, T>() ||
         is_instantiation<std::unordered_multimap, T>();
}

} // namespace utils
