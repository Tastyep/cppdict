#ifndef CPPDICT_ENTRY_HPP
#define CPPDICT_ENTRY_HPP

#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "attr.hpp"
#include "concepts.hpp"
#include "stringLiteral.hpp"

template<StringLiteral EntryName, typename Type, typename... Attributes>
class Entry {
 public:
  using type = Type;

  constexpr explicit Entry(Type value, Attributes&&... attrs)
    : value(std::move(value))
    , attrs(std::forward<Attributes>(attrs)...) {}

  template<StringLiteral Path>
  constexpr auto& get() {
    return this->get<Path, std::true_type>();
  }

  template<StringLiteral Path, Detail::Invalid PathParsing>
  constexpr auto& get() {
    constexpr std::string_view path = Path;
    constexpr bool attrKey = path.starts_with('@');
    return this->get<std::bool_constant<attrKey>, Path>();
  }

 private:
  // Parse the next entry name from Path
  template<StringLiteral Path, Detail::Valid PathParsing>
  constexpr auto& get() {
    constexpr std::string_view view = Path;
    constexpr auto pos = view.find('/');
    constexpr auto key = view.substr(0, pos);
    constexpr auto path = view.substr(pos == view.npos ? view.size() : pos + 1);
    constexpr bool nameMatch = EntryName == key;
    return this->get<std::bool_constant<nameMatch>,
                     std::bool_constant<!path.empty()>,
                     StringLiteral<path.length()>(path.data())>();
  }

  // Matching entry name & non empty Path
  template<Detail::Valid Match, Detail::Valid PathLength, StringLiteral Path>
  constexpr auto& get() {
    return this->get<Path>(this->value);
  }

  // Matching entry name & empty Path
  template<Detail::Valid Match, Detail::Invalid PathLength, StringLiteral Path>
  constexpr auto& get() {
    return this->value;
  }

  // Parameter pack lookup
  template<StringLiteral Path, typename... Entries>
  constexpr auto& get(std::tuple<Entries...>& entries) {
    constexpr std::string_view view = Path;
    constexpr auto pos = view.find('/');
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

  // Match entry name against Key
  template<StringLiteral Key,
           typename PathLength,
           StringLiteral Path,
           StringLiteral Name,
           typename T,
           typename... Attrs,
           typename... Entries>
  constexpr auto& get(Entry<Name, T, Attrs...>& entry, Entries&... entries) {
    constexpr bool nameMatch = Name == Key;
    return this->get<std::bool_constant<nameMatch>, Key, PathLength, Path>(entry, entries...);
  }

  // Matched entry, non empty Path
  template<Detail::Valid Match,
           StringLiteral Key,
           Detail::Valid PathLength,
           StringLiteral Path,
           typename T,
           typename... Attrs,
           typename... Entries>
  constexpr auto& get(Entry<Key, T, Attrs...>& entry, Entries&... entries) {
    return entry.template get<Path, std::false_type>();
  }

  // Matched entry, empty Path
  template<Detail::Valid Match,
           StringLiteral Key,
           Detail::Invalid PathLength,
           StringLiteral Path,
           typename Entry,
           typename... Entries>
  constexpr auto& get(Entry& entry, Entries&... entries) {
    return entry.value;
  }

  // Invalid match, tuple lookup
  template<Detail::Invalid Match,
           StringLiteral Key,
           typename PathLength,
           StringLiteral Path,
           typename Entry,
           typename... Entries>
  constexpr auto& get(Entry& entry, Entries&... entries) {
    return this->get<Key, PathLength, Path>(entries...);
  }

  // Entry referencing Path, parse it and inspect the entrie's value
  template<Detail::Invalid AttrKey, StringLiteral Path>
  constexpr auto& get() {
    return this->get<Path>(this->value);
  }

  // Attribute key & non empty Path
  template<Detail::Valid AttrKey, StringLiteral Path>
  constexpr auto& get() {
    constexpr std::string_view path = Path;
    constexpr auto attrKey = path.substr(1); // Skip the @ prefix
    constexpr auto attrKeyLiteral = StringLiteral<attrKey.length()>(attrKey.data());
    return std::apply(
      [this, &attrKeyLiteral](auto&... attrs) -> auto& {
        return this->getAttr<attrKeyLiteral>(attrs...);
      },
      this->attrs);
  }

  template<StringLiteral AttrKey, StringLiteral Name, typename T, typename... Attrs>
  constexpr auto& getAttr(Attr<Name, T>& attr, Attrs&... attrs) {
    constexpr auto nameMatch = AttrKey == Name;
    return this->getAttr<std::bool_constant<nameMatch>, Name>(attr, attrs...);
  }

  template<Detail::Valid NameMatch, StringLiteral AttrKey, typename Attr, typename... Attrs>
  constexpr auto& getAttr(Attr& attr, Attrs&... attrs) {
    return attr.value;
  }

  template<Detail::Invalid NameMatch, StringLiteral AttrKey, typename Attr, typename... Attrs>
  constexpr auto& getAttr(Attr& attr, Attrs&... attrs) {
    return this->getAttr<AttrKey>(attrs...);
  }

  // Invalid cases

  template<StringLiteral Key, typename PathLength, StringLiteral Path>
  constexpr auto& get() {
    static_assert(Key.empty(), "Key not found in tuple");
  }

  template<StringLiteral Path>
  constexpr auto& get(auto& value) {
    static_assert(Path.empty(),
                  "No match for Path, cannot perform lookup on non tuple entry type");
  }

  template<StringLiteral AttrKey>
  constexpr auto& getAttr() {
    static_assert(AttrKey.empty(), "Attribute key not found in tuple");
  }

 public:
  static constexpr std::string_view name = EntryName;
  Type value;
  std::tuple<Attributes...> attrs;
};

template<StringLiteral Name, typename Type, typename... Attrs>
constexpr auto makeEntry(const Type& type, Attrs&&... attrs) {
  return Entry<Name, Type, Attrs...>(type, std::forward<Attrs>(attrs)...);
}

template<StringLiteral Name, typename... Attrs>
constexpr auto makeEntry(const char* str, Attrs&&... attrs) {
  return Entry<Name, std::string, Attrs...>(std::string(str), std::forward<Attrs>(attrs)...);
}

#endif // !CPPDICT_ENTRY_HPP
