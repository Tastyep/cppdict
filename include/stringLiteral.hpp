#ifndef CPPDICT_STRING_LITERAL_HPP
#define CPPDICT_STRING_LITERAL_HPP

#include <array>
#include <string_view>

template<unsigned N>
struct StringLiteral {
  using UnderlyingType = std::array<char, N + 1>;

  constexpr StringLiteral(const char* str) {
    std::copy(str, str + N, value.begin());
    value[N] = 0;
  }

  constexpr StringLiteral(std::string_view str) {
    std::copy(str.begin(), str.end(), value);
  }

  constexpr bool empty() const {
    return value.empty();
  }

  constexpr std::string_view view() const {
    return value.data();
  }

  constexpr operator std::string_view() const {
    return this->view();
  }

  template<unsigned Size>
  constexpr bool operator==(const StringLiteral<Size>& other) const {
    return this->view() == other.view();
  }

  UnderlyingType value{};
};

template<unsigned N>
StringLiteral(const char (&)[N]) -> StringLiteral<N - 1>;

#endif // !CPPDICT_STRING_LITERAL_HPP
