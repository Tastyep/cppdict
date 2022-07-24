#ifndef CPPDICT_ENTRY_HPP
#define CPPDICT_ENTRY_HPP

#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "concepts.hpp"
#include "stringLiteral.hpp"

template<StringLiteral EntryName, typename Type>
class Entry {
 public:
  using type = Type;
  static constexpr std::string_view name = EntryName;

  constexpr explicit Entry(Type value)
    : value(std::move(value)) {}

  template<StringLiteral Path>
  constexpr auto& get() {
    return this->get<Path, std::true_type>();
  }

  template<StringLiteral Path, Detail::Invalid PathParsing>
  constexpr auto& get() {
    return this->get<Path>(this->value);
  }

 private:
  template<StringLiteral Path, Detail::Valid PathParsing>
  constexpr auto& get() {
    constexpr std::string_view view = Path;
    constexpr auto pos = view.find('.');
    constexpr auto key = view.substr(0, pos);
    constexpr auto path = view.substr(pos == view.npos ? view.size() : pos + 1);
    constexpr bool nameMatch = EntryName == key;
    return this->get<std::bool_constant<nameMatch>,
                     std::bool_constant<!path.empty()>,
                     StringLiteral<path.length()>(path.data())>();
  }

  template<Detail::Valid Match, Detail::Valid PathLength, StringLiteral Path>
  constexpr auto& get() {
    return this->get<Path>(this->value);
  }

  template<Detail::Valid Match, Detail::Invalid PathLength, StringLiteral Path>
  constexpr auto& get() {
    return this->value;
  }

  template<StringLiteral Path, typename... Entries>
  constexpr auto& get(std::tuple<Entries...>& entries) {
    constexpr std::string_view view = Path;
    constexpr auto pos = view.find('.');
    constexpr auto name = view.substr(0, pos);
    constexpr auto path = view.substr(pos == view.npos ? view.size() : pos + 1);

    return std::apply( //
      [this, &name, &path](auto&... entries) -> auto& {
        return this->get<StringLiteral<name.length()>(name.data()),
                         std::bool_constant<!path.empty()>,
                         StringLiteral<path.length()>(path.data())>(entries...);
      },
      entries);
  }

  template<StringLiteral Key,
           typename PathLength,
           StringLiteral Path,
           StringLiteral Name,
           typename T,
           typename... Entries>
  constexpr auto& get(Entry<Name, T>& entry, Entries&... entries) {
    constexpr bool nameMatch = Name == Key;
    return this->get<std::bool_constant<nameMatch>, Key, PathLength, Path>(entry, entries...);
  }

  template<Detail::Valid Match,
           StringLiteral Key,
           Detail::Valid PathLength,
           StringLiteral Path,
           typename Entry,
           typename... Entries>
  constexpr auto& get(Entry& entry, Entries&... entries) {
    return entry.template get<Path, std::false_type>();
  }

  template<Detail::Valid Match,
           StringLiteral Key,
           Detail::Invalid PathLength,
           StringLiteral Path,
           typename Entry,
           typename... Entries>
  constexpr auto& get(Entry& entry, Entries&... entries) {
    return entry.value;
  }

  template<Detail::Invalid Match,
           StringLiteral Key,
           typename PathLength,
           StringLiteral Path,
           typename Entry,
           typename... Entries>
  constexpr auto& get(Entry& entry, Entries&... entries) {
    return this->get<Key, PathLength, Path>(entries...);
  }

  // Invalid cases

  template<StringLiteral Key, typename PathLength, StringLiteral Path>
  constexpr auto& get() {
    static_assert(Key.empty(), "Key not found in tuple");
  }

 public:
  Type value;
};

template<StringLiteral Name, typename Type>
constexpr auto makeEntry(const Type& type) {
  return Entry<Name, Type>(type);
}

template<StringLiteral Name>
constexpr auto makeEntry(const char* str) {
  return Entry<Name, std::string>(std::string(str));
}

#endif // !CPPDICT_ENTRY_HPP
