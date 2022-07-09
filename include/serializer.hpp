#ifndef CPPDICT_SERIALIZER_HPP
#define CPPDICT_SERIALIZER_HPP

#include <tuple>
#include <utility>

#include "entry.hpp"

template<typename Writer>
class Serializer {
 public:
  Serializer(Writer writer)
    : _writer(std::move(writer)) {}

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
    _writer.writeStartElement(entry.name);
    std::apply([this](const auto&... entries) { this->serialize(entries...); }, entry.value);
    _writer.writeEndElement();
  }

  template<typename T>
  void serializeEntry(const Entry<T>& entry) {
    _writer.write(entry.name, entry.value);
  }

 private:
  Writer _writer;
};

template<typename Writer>
Serializer(Writer) -> Serializer<Writer>;

#endif // !CPPDICT_SERIALIZER_HPP
