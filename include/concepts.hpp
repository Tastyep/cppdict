#ifndef CPPDICT_CONCEPTS_HPP
#define CPPDICT_CONCEPTS_HPP

#include <concepts>
#include <ranges>
#include <string>

template<typename T, typename Serializer>
concept Serializable = requires(T obj, Serializer& serializer) {
  obj.serialize(serializer);
};

template<typename T, typename Deserializer>
concept Deserializable = requires(T obj, Deserializer& deserializer) {
  obj.deserialize(deserializer);
};

template<typename T>
concept Collection = std::ranges::range<T> and not std::is_same_v<std::string, T>;

template<typename T, typename... U>
concept IsAnyOf = (std::same_as<T, U> || ...);

#endif // !CPPDICT_CONCEPTS_HPP
