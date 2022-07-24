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
  template<typename Entry, typename... Entries>
  void serialize(const Entry& entry, const Entries&... entries) {
    this->serializeEntry(entry);
    if constexpr (sizeof...(entries) > 0) {
      this->serialize(entries...);
    }
  }

 private:
  template<StringLiteral Name, typename... Entries>
  void serializeEntry(const Entry<Name, std::tuple<Entries...>>& entry) {
    _writer.writeObjStartElement(entry.name);
    std::apply([this](const auto&... entries) { this->serialize(entries...); }, entry.value);
    _writer.writeObjEndElement();
  }

  template<StringLiteral Name, Detail::Collection Range>
  void serializeEntry(const Entry<Name, Range>& entry) {
    _writer.writeArrayStartElement(entry.name);
    for (const auto& entry : entry.value) {
      this->serializeEntry(entry);
    }
    _writer.writeArrayEndElement();
  }

  template<StringLiteral Name, typename T>
  requires(!Serializable<T, Serializer<Writer>> &&
           !Detail::Collection<T>) void serializeEntry(const Entry<Name, T>& entry) {
    _writer.write(entry.name, entry.value);
  }

  template<StringLiteral Name, Serializable<Serializer<Writer>> T>
  void serializeEntry(const Entry<Name, T>& entry) {
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
