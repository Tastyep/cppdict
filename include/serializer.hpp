#ifndef CPPDICT_SERIALIZER_HPP
#define CPPDICT_SERIALIZER_HPP

#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "concepts.hpp"
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
    _writer.writeObjStartElement(entry.name);
    std::apply([this](const auto&... entries) { this->serialize(entries...); }, entry.value);
    _writer.writeObjEndElement();
  }

  template<Collection Range>
  void serializeEntry(const Entry<Range>& entry) {
    _writer.writeArrayStartElement(entry.name);
    for (const auto& entry : entry.value) {
      this->serializeEntry(entry);
    }
    _writer.writeArrayEndElement();
  }

  template<typename T>
  void serializeEntry(const Entry<T>& entry) {
    _writer.write(entry.name, entry.value);
  }

  template<Serializable<Serializer<Writer>> T>
  void serializeEntry(const Entry<T>& entry) {
    _writer.writeObjStartElement(entry.name);
    entry.value.serialize(*this);
    _writer.writeObjEndElement();
  }

  void serializeEntry(const Serializable<Serializer<Writer>> auto& entry) {
    _writer.writeObjStartElement(entry.EntryName);
    entry.serialize(*this);
    _writer.writeObjEndElement();
  }

  void serializeEntry(const auto& entry) {
    _writer.writeValue(entry);
  }

 private:
  Writer _writer;
};

template<typename Writer>
Serializer(Writer) -> Serializer<Writer>;

#endif // !CPPDICT_SERIALIZER_HPP
