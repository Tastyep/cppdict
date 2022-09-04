#ifndef CPPDICT_SERIALIZER_HPP
#define CPPDICT_SERIALIZER_HPP

#include <bits/utility.h>
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
  // Tuple entry
  template<StringLiteral Name, typename... Entries>
  void serializeEntry(const Entry<Name, std::tuple<Entries...>>& entry) {
    _writer.writeObjStartElement(entry.name);
    std::apply([this](const auto&... entries) { this->serialize(entries...); }, entry.value);
    _writer.writeObjEndElement();
  }

  // Collection entry
  template<StringLiteral Name, Detail::Collection Range>
  void serializeEntry(const Entry<Name, Range>& entry) {
    _writer.writeArrayStartElement(entry.name);
    for (const auto& entry : entry.value) {
      this->serializeEntry(entry);
    }
    _writer.writeArrayEndElement();
  }

  // Simple entry
  template<StringLiteral Name, typename T, typename... Attrs>
  requires(!Serializable<T, Serializer<Writer>> &&
           !Detail::Collection<T>) void serializeEntry(const Entry<Name, T, Attrs...>& entry) {
    // TODO: provide _writer.write(entry.name)
    _writer.write(entry.name, entry.value);
    if constexpr (sizeof...(Attrs) > 0) {
      this->serializeAttributes(entry.attrs);
    }
  }

  // Serializable object entry
  template<StringLiteral Name, Serializable<Serializer<Writer>> T, typename... Attrs>
  void serializeEntry(const Entry<Name, T, Attrs...>& entry) {
    _writer.writeObjStartElement(entry.name);
    if constexpr (sizeof...(Attrs) > 0) {
      this->serializeAttributes(entry.attrs);
    }
    entry.value.serialize(*this);
    _writer.writeObjEndElement();
  }

  // Serializale object
  void serializeEntry(const Serializable<Serializer<Writer>> auto& entry) {
    _writer.writeObjStartElement(entry.EntryName);
    entry.serialize(*this);
    _writer.writeObjEndElement();
  }

  void serializeEntry(const auto& entry) {
    _writer.writeValue(entry);
  }

  template<typename... Attrs>
  void serializeAttributes(std::tuple<Attrs...> attrTuple) {
    _writer.writeAttrStartElement();
    std::apply([this](auto&... attrs) { this->serializeAttributes(attrs...); }, attrTuple);
    _writer.writeAttrEndElement();
  }

  template<typename Attr, typename... Attrs>
  void serializeAttributes(const Attr& attr, const Attrs&... attrs) {
    _writer.writeAttr(attr.name, attr.value);
    if constexpr (sizeof...(attrs) > 0) {
      this->serializeAttributes(attrs...);
    }
  }

 private:
  Writer _writer;
};

template<typename Writer>
Serializer(Writer) -> Serializer<Writer>;

#endif // !CPPDICT_SERIALIZER_HPP
