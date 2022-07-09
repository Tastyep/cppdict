#ifndef CPPDICT_ENTRY_HPP
#define CPPDICT_ENTRY_HPP

#include <string_view>
#include <utility>

template<typename Type>
struct Entry {
  using type = Type;

  constexpr Entry() = default;

  template<typename... Args>
  constexpr explicit Entry(std::string_view name, Args&&... args)
    : name(name)
    , value(std::forward<Args>(args)...) {}

  std::string_view name;
  Type value;
};

// Deduction guides

template<typename Arg>
Entry(std::string_view, const Arg&) -> Entry<Arg>;

template<unsigned N>
Entry(std::string_view, const char (&)[N]) -> Entry<std::string>;

#endif // !CPPDICT_ENTRY_HPP
