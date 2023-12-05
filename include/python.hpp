/**********************************************************************
 * @brief Various pythonic utilities
 * @details
 * @author Sam Tonetto
 * @copyright GNU Public License
 * @date 2023
 ***********************************************************************/

#pragma once

#include <iterator>

namespace utils {

namespace detail {
template <typename Container, bool IsConst>
class enumerator_base {
private:
  using enumerator_type =
      std::conditional_t<IsConst, typename Container::const_iterator,
                         typename Container::iterator>;

  using elem_type =
      std::conditional_t<IsConst, const typename Container::value_type,
                         typename Container::value_type>;

public:
  using iterator_category = std::forward_iterator_tag;

  // reference wrapper is necessary since references are not assignable
  using value_type = std::pair<std::size_t, std::reference_wrapper<elem_type>>;

  using difference_type = std::ptrdiff_t;
  using reference = value_type &;

public:
  enumerator_base(std::size_t i, enumerator_type it)
      : index{i}, it{it}, value{i, *it} {}

  enumerator_base &operator++() {
    ++index;
    ++it;
    value = {index, *it};
    return *this;
  }

  bool operator!=(const enumerator_base &other) const { return it != other.it; }

  reference operator*() { return value; }

private:
  std::size_t index;
  enumerator_type it;

  value_type value;
};

} // namespace detail

template <typename Container>
using const_enumerator = detail::enumerator_base<Container, true>;

template <typename Container>
using enumerator = detail::enumerator_base<Container, false>;

// Non-const enumerate
template <typename Container>
struct enumerate {

  Container &container;

  enumerate(Container &container) : container{container} {}

  enumerator<Container> begin() { return {0, std::begin(container)}; }
  enumerator<Container> end() {
    return {std::size(container), std::end(container)};
  }
};

// Const enumerate
template <typename Container>
struct enumerate<const Container> {

  const Container &container;
  enumerate(const Container &container) : container{container} {}

  const_enumerator<Container> begin() const {
    return {0, std::cbegin(container)};
  }
  const_enumerator<Container> end() const {
    return {std::size(container), std::cend(container)};
  }
};

} // namespace utils
