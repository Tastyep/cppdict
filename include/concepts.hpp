#ifndef CPPDICT_CONCEPTS_HPP
#define CPPDICT_CONCEPTS_HPP

#include <concepts>
#include <ranges>
#include <string>
#include <type_traits>

template<typename T, typename Serializer>
concept Serializable = requires(T obj, Serializer& serializer) {
  obj.serialize(serializer);
};

template<typename T, typename Deserializer>
concept Deserializable = requires(T obj, Deserializer& deserializer) {
  obj.deserialize(deserializer);
};

namespace Detail {

  template<typename T>
  concept Valid = std::same_as<T, std::true_type>;

  template<typename T>
  concept Invalid = std::same_as<T, std::false_type>;

  template<typename T>
  concept IsTuple = requires(T value) {
    std::tuple_size<T>{};
  };

  template<typename T>
  concept Collection =
    std::ranges::range<T> and not std::is_same_v<std::string, T> and requires(T value) {
    value.emplace_back();
  };

  template<typename T, typename... U>
  concept IsAnyOf = (std::same_as<T, U> || ...);

} // namespace Detail

#endif // !CPPDICT_CONCEPTS_HPP
