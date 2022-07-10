#ifndef CPPDICT_DESERIALIZER
#define CPPDICT_DESERIALIZER

#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "concepts.hpp"
#include "entry.hpp"

template<typename Reader>
class Deserializer {
 public:
  Deserializer(Reader reader)
    : _reader(std::move(reader)) {}
  template<typename... Entries>
  void deserialize(Entries&&... entries) {
    auto [name, eos] = _reader.nextEntryName();

    while (!eos) {
      if (!this->findNamedEntry(name, entries...)) {
        std::cout << "Not found" << std::endl;
      };
      std::tie(name, eos) = _reader.nextEntryName();
    }
  }

 private:
  template<typename Entry, typename... Entries>
  bool findNamedEntry(const std::string& name, Entry& entry, Entries&... entries) {
    if (this->matchEntry(entry, name)) {
      this->processEntry(entry);
      return true;
    }

    if constexpr (sizeof...(entries) > 0) {
      return this->findNamedEntry(name, entries...);
    }
    return false;
  }

  template<typename T>
  bool matchEntry(const Entry<T>& entry, const std::string& name) const {
    return name == entry.name;
  }

  bool matchEntry(Deserializable<Deserializer<Reader>> auto& entry,
                  const std::string& name) const {
    return name == entry.EntryName;
  }

  template<typename... Args>
  void processEntry(Entry<std::tuple<Args...>>& entry) {
    std::cout << entry.name << " {" << std::endl;
    std::apply([this](auto&... entries) { this->deserialize(entries...); }, entry.value);
  }

  template<typename T>
  void processEntry(Entry<T>& entry) {
    std::cout << entry.name << ": ";
    this->processEntry(entry.value);
  }

  template<Deserializable<Deserializer<Reader>> T>
  void processEntry(Entry<T>& entry) {
    std::cout << entry.name << " {" << std::endl;
    entry.value.deserialize(*this);
  }

  template<typename T>
  void processEntry(std::reference_wrapper<T>& entry) {
    this->processEntry(entry.get());
  }

  void processEntry(Deserializable<Deserializer<Reader>> auto& entry) {
    std::cout << entry.EntryName << " {" << std::endl;
    entry.deserialize(*this);
  }

  void processEntry(Collection auto& entries) {
    using T = typename std::decay<decltype(*entries.begin())>::type;
    std::cout << "[" << std::endl;
    while (_reader.loadNextEntry()) {
      T newEntry;
      this->processEntry(newEntry);
      entries.push_back(std::move(newEntry));
    }
  }

  void processEntry(auto& entry) {
    _reader.value(entry);
  }

 private:
  Reader _reader;
};

template<typename Reader>
Deserializer(Reader) -> Deserializer<Reader>;

#endif // !CPPDICT_DESERIALIZER
