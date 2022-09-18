#ifndef CPPDICT_DESERIALIZER
#define CPPDICT_DESERIALIZER

#include <functional>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
#include <iostream>

#include "concepts.hpp"
#include "entry.hpp"

template<typename Reader>
class Deserializer {
 public:
  Deserializer(Reader reader)
    : _reader(std::move(reader)) {}

  template<typename... Entries>
  void deserialize(Entries&&... entries) {
    auto [name, valid] = _reader.nextEntryName();

    while (valid) {
      if (!this->findEntry(name, entries...)) {
        std::cerr << "Entry '" << name << "' not found" << std::endl;
      };
      std::tie(name, valid) = _reader.nextEntryName();
    }
  }

 private:
  // Lookup entries by name
  template<typename Entry, typename... Entries>
  bool findEntry(const std::string& name, Entry& entry, Entries&... entries) {
    if (this->matchEntry(entry, name)) {
      this->processEntry(entry);
      return true;
    }

    if constexpr (sizeof...(entries) > 0) {
      return this->findEntry(name, entries...);
    }
    return false;
  }

  // Match entry by name
  template<StringLiteral Name, typename T, typename... Attrs>
  bool matchEntry(const Entry<Name, T, Attrs...>& entry, const std::string& name) const {
    return name == entry.name;
  }

  // Match Deserializable user class by name
  bool matchEntry(Deserializable<Deserializer<Reader>> auto& entry,
                  const std::string& name) const {
    return name == entry.EntryName;
  }

  // Process a tuple entry
  template<StringLiteral Name, typename... Args, typename... Attrs>
  void processEntry(Entry<Name, std::tuple<Args...>, Attrs...>& entry) {
    std::apply([this](auto&... attrs) { this->processAttrs(attrs...); }, entry.attrs);
    std::apply([this](auto&... entries) { this->deserialize(entries...); }, entry.value);
  }

  // Process a simple entry
  template<StringLiteral Name, typename T, typename... Attrs>
  void processEntry(Entry<Name, T, Attrs...>& entry) {
    std::apply([this](auto&... attrs) { this->processAttrs(attrs...); }, entry.attrs);
    this->processEntry(entry.value);
  }

  // Process an entry containing a Deserializable user class
  template<StringLiteral Name, Deserializable<Deserializer<Reader>> T, typename... Attrs>
  void processEntry(Entry<Name, T, Attrs...>& entry) {
    std::apply([this](auto&... attrs) { this->processAttrs(attrs...); }, entry.attrs);
    entry.value.deserialize(*this);
  }

  // Process a reference wrapper
  template<typename T>
  void processEntry(std::reference_wrapper<T>& entry) {
    this->processEntry(entry.get());
  }

  // Process a Deserializable user class
  void processEntry(Deserializable<Deserializer<Reader>> auto& entry) {
    entry.deserialize(*this);
  }

  // Process a collection
  void processEntry(Detail::Collection auto& entries) {
    using T = typename std::decay<decltype(*entries.begin())>::type;
    while (_reader.nextArrayEntry()) {
      T newEntry;
      this->processEntry(newEntry);
      entries.push_back(std::move(newEntry));
    }
  }

  template<typename... Attrs>
  void processAttrs(Attrs&... attrs) {
    if constexpr (sizeof...(Attrs) > 0) {
      this->processAttr(attrs...);
    }
  }

  template<StringLiteral Name, typename T, typename... Attrs>
  void processAttr(Attr<Name, T>& attr, Attrs&... attrs) {
    _reader.attrValue(attr.name, attr.value);
    this->processAttrs(attrs...);
  }

  // Fallback
  void processEntry(auto& entry) {
    _reader.value(entry);
  }

 private:
  Reader _reader;
};

template<typename Reader>
Deserializer(Reader) -> Deserializer<Reader>;

#endif // !CPPDICT_DESERIALIZER
