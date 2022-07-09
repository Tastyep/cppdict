#ifndef CPPDICT_SERIALIZER_HPP
#define CPPDICT_SERIALIZER_HPP

#include <tuple>
#include <utility>

#include "entry.hpp"

template<auto Writer>
class Serializer {
 public:
  template<typename Entry, typename... Args>
  void serialize(const Entry& entry, const Args&... args) {
    this->serializeEntry(entry);
    if constexpr (sizeof...(args) > 0) {
      this->serialize(args...);
    }
  }

 private:
  template<typename... Args>
  void serializeEntry(const Entry<std::tuple<Args...>>& entry) {
    Writer.writeStartElement(entry.name);
    std::apply([this](const auto&... entries) { this->serialize(entries...); }, entry.value);
    Writer.writeEndElement();
  }

  template<typename T>
  void serializeEntry(const Entry<T>& entry) {
    Writer.write(entry.name, entry.value);
  }
};

#endif // !CPPDICT_SERIALIZER_HPP
