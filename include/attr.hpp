#ifndef CPPDICT_ATTR_HPP
#define CPPDICT_ATTR_HPP

#include <string_view>
#include <utility>

#include "stringLiteral.hpp"

template<StringLiteral AttrName, typename Type>
class Attr {
 public:
  using type = Type;

  constexpr Attr(Type value)
    : value(std::move(value)) {}

 public:
  static constexpr std::string_view name = AttrName;
  Type value;
};

template<StringLiteral Name, typename Type>
constexpr auto makeAttr(const Type& type) {
  return Attr<Name, Type>(type);
}

#endif // !CPPDICT_ATTR_HPP
