#ifndef CPPDICT_DESERIALIZER
#define CPPDICT_DESERIALIZER

#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "entry.hpp"

template<typename Reader>
class Deserializer {
 public:
  Deserializer(Reader reader)
    : _reader(std::move(reader)) {}

  template<typename... Entries>
  void deserialize(Entries&... entries) const {
    auto [name, eos] = _reader.nextEntryName();

    while (!eos) {
      if (!this->processNamedEntries(name, entries...)) {
        std::cout << "Not found" << std::endl;
      };
      std::tie(name, eos) = _reader.nextEntryName();
    }
  }

 private:
  template<typename Entry, typename... Entries>
  bool processNamedEntries(const std::string& name, Entry& entry, Entries&... entries) const {
    if (name == entry.name) {
      this->processEntry(entry);
      return true;
    }

    if constexpr (sizeof...(entries) > 0) {
      return this->processNamedEntries(name, entries...);
    }
    return false;
  }

  template<typename... Args>
  void processEntry(Entry<std::tuple<Args...>>& entry) const {
    std::cout << entry.name << " {" << std::endl;
    std::apply([this](auto&... entries) { this->deserialize(entries...); }, entry.value);
  }

  template<typename T>
  void processEntry(Entry<std::vector<T>>& entry) const {
    std::cout << "[" << std::endl;
    auto& entries = entry.value;
    while (_reader.loadNextEntry()) {
      T newEntry;
      this->processEntry(newEntry);
      entries.push_back(std::move(newEntry));
    }
  }

  template<typename T>
  void processEntry(Entry<T>& entry) const {
    std::cout << entry.name << ": ";
    this->processEntry(entry.value);
  }

  void processEntry(auto& entry) const {
    _reader.value(entry);
  }

 private:
  Reader _reader;
};

template<typename Reader>
Deserializer(Reader) -> Deserializer<Reader>;

#endif // !CPPDICT_DESERIALIZER
