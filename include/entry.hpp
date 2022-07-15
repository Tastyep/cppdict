#ifndef CPPDICT_ENTRY_HPP
#define CPPDICT_ENTRY_HPP

#include <functional>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include "concepts.hpp"

template<typename Type>
struct Entry {
  using type = Type;

  constexpr explicit Entry(std::string_view name, Type value)
    : name(name)
    , value(std::move(value)) {}

  template<typename T>
  std::optional<T> get(const char* path) {
    return this->get<T>(std::string_view(path));
  }

  template<typename T>
  std::optional<T> get(std::string_view path) {
    const auto pos = path.find('.');

    if (pos == path.npos) {
      return path == this->name //
               ? this->get<T>(this->value)
               : std::optional<T>();
    }

    const auto token = path.substr(0, pos);
    return token == this->name //
             ? this->get<T>(path.substr(pos + 1), this->value)
             : std::optional<T>();
  }

 private:
  template<typename T, typename... Entries>
  std::optional<T> get(std::string_view path, std::tuple<Entries...>& entries) {
    const auto pos = path.find('.');

    if (pos == path.npos) {
      return std::apply( //
        [this, &path](auto&... entries) { return this->get<T>(path, entries...); },
        entries);
    }

    const auto token = path.substr(0, pos);
    return std::apply( //
      [this, &token, &path](auto&... entries) {
        return this->get<T>(token, path, entries...);
      },
      entries);
  }

  template<typename T, typename U, typename... Entries>
  std::optional<T> get(std::string_view token, Entry<U>& entry, Entries&... entries) {
    auto result = entry.template get<T>(token);
    if (result) {
      return result;
    }

    if constexpr (sizeof...(entries) > 0) {
      return this->get<T>(token, entries...);
    }
    return {};
  }

  template<typename T, typename U, typename... Entries>
  std::optional<T> get(std::string_view token, std::string_view path, Entry<U>& entry,
                       Entries&... entries) {
    if (entry.name == token) {
      return entry.template get<T>(path);
    }
    if constexpr (sizeof...(entries) > 0) {
      return this->get<T>(token, path, entries...);
    }
    return {};
  }

  template<typename T>
  std::optional<T> get(Entry<T>& entry) {
    return entry.value;
  }

  template<typename T>
  std::optional<T> get(T& value) {
    return value;
  }

  template<typename T, typename U>
  std::optional<T> get(U& value) requires(std::is_same_v<T, std::reference_wrapper<U>>) {
    return std::ref(value);
  }

  template<typename T, typename U>
  std::optional<T> get(U& entry) requires(!IsAnyOf<T, U, std::reference_wrapper<U>>) {
    return {};
  }

  template<typename T, typename U>
  std::optional<T> get(std::string_view path, U& entry) {
    return {};
  }

  template<typename T>
  std::optional<T> get(std::string_view token, std::string_view path, T& entry) {
    return {};
  }

 public:
  std::string_view name;
  Type value;
};

// Deduction guides

template<typename Type>
Entry(std::string_view, const Type&) -> Entry<Type>;

template<unsigned N>
Entry(std::string_view, const char (&)[N]) -> Entry<std::string>;

#endif // !CPPDICT_ENTRY_HPP
