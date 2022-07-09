#ifndef CPPDICT_DESERIALIZER
#define CPPDICT_DESERIALIZER

#include <iostream>
#include <string>
#include <tuple>
#include <utility>

#include "entry.hpp"

template<auto Reader>
class Deserializer {
 public:
  template<typename... Entries>
  void deserialize(Entries&... entries) const {
    auto [name, eos] = Reader.read();

    while (!eos && name != "-") {
      if (!this->processNamedEntries(name, entries...)) {
        std::cout << "Not found" << std::endl;
      };
      std::tie(name, eos) = Reader.read();
    }
  }

 private:
  template<typename Entry, typename... Entries>
  bool processNamedEntries(const std::string& name, Entry& entry, Entries&... entries) const {
    if (this->processNamedEntry(name, entry)) {
      return true;
    }
    if constexpr (sizeof...(entries) > 0) {
      return this->processNamedEntries(name, entries...);
    }
    return false;
  }

  template<typename... Args>
  bool processNamedEntry(const std::string& name, Entry<std::tuple<Args...>>& entry) const {
    if (entry.name == name) {
      std::cout << name << " {" << std::endl;
      this->deserializeTuple(entry.value, std::make_index_sequence<sizeof...(Args)>{});
      return true;
    }

    return false;
  }

  template<typename T>
  bool processNamedEntry(const std::string& name, Entry<T>& entry) const {
    if (entry.name == name) {
      std::cout << name << ": ";
      Reader.value(entry.value);
      return true;
    }

    return false;
  }

  template<typename Entries, std::size_t... Is>
  void deserializeTuple(Entries& entries, [[maybe_unused]] std::index_sequence<Is...> seq) const {
    this->deserialize(get<Is>(entries)...);
  }
};

#endif // !CPPDICT_DESERIALIZER
