#ifndef CPPDICT_CONSOLE_READ_WRITER_HPP
#define CPPDICT_CONSOLE_READ_WRITER_HPP

#include <concepts>
#include <ios>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

#include "concepts.hpp"
#include "serializer.hpp"

std::string readInput(std::string_view prefix) {
  std::string input;
  std::cout << prefix;
  std::cin >> input;

  return input;
}

template<typename T>
concept Unspecified = not std::integral<T> and not Detail::IsAnyOf<T, std::string, bool>;

struct CoutWriter {
 public:
  void write(std::string_view name, std::integral auto value) {
    std::cout << _indent << "'" << name << "' (integral) : " << value << std::endl;
  }

  void write(std::string_view name, bool value) {
    std::cout << _indent << "'" << name << "' (bool) : " << std::boolalpha << value
              << std::endl;
  }

  void write(std::string_view name, const std::string& value) {
    std::cout << _indent << "'" << name << "' (string) : "
              << "\"" << value << "\"" << std::endl;
  }

  void write(std::string_view name, const Unspecified auto& value) {
    std::cout << _indent << "'" << name << "' (T) : " << value << std::endl;
  }

  void write(std::string_view name) {
    std::cout << _indent << "'" << name << "' (void)" << std::endl;
  }

  template<typename T>
  void writeValue(const T& value) {
    std::cout << _indent << value << std::endl;
  }

  void writeObjStartElement(std::string_view name) {
    std::cout << _indent << "'" << name << "' : {" << std::endl;
    this->addIndent(2);
  }

  void writeObjEndElement() {
    this->addIndent(-2);
    std::cout << _indent << "}" << std::endl;
  }

  void writeArrayStartElement(std::string_view name) {
    std::cout << _indent << "'" << name << "' : [" << std::endl;
    this->addIndent(2);
  }

  void writeArrayEndElement() {
    this->addIndent(-2);
    std::cout << _indent << "]" << std::endl;
  }

  void addIndent(int amount) {
    int indent = _indent.size() + amount;
    _indent = std::string(indent, ' ');
  }

  void writeAttr(std::string_view name, const auto& value) {
    std::cout << "'" << name << "': " << value;
  }

  void writeAttrStartElement() {
    std::cout << _indent << "  -> ATTRS: [ ";
  }

  void writeAttrEndElement() {
    std::cout << " ]" << std::endl;
  }

 private:
  std::string _indent;
};

struct CinReader {
  static constexpr std::string_view prefix = "value: ";

  [[nodiscard]] std::pair<std::string, bool> nextEntryName() const {
    std::string str;

    std::cin >> str;
    const bool noValue = std::cin.eof() || str == "-";

    return { std::move(str), noValue };
  }

  bool loadNextEntry() const {
    std::cout << "Next Entry [+/-]: ";
    const auto& [_, eof] = this->nextEntryName();
    return !eof;
  }

  bool value(auto& value) const {
    std::cout << "Value: ";
    const auto& [input, eof] = this->nextEntryName();
    if (eof) {
      return false;
    }

    this->setValue(value, input);
    return true;
  }

  bool attrValue(std::string_view name, auto& value) const {
    std::cout << "Attr '" << name << "': ";
    const auto& [inputValue, eof] = this->nextEntryName();
    if (eof) {
      return false;
    }

    this->setValue(value, inputValue);
    return true;
  }

 private:
  void setValue(std::integral auto& value, const std::string& input) const {
    value = std::stoi(input);
  }

  void setValue(bool& value, const std::string& input) const {
    value = input == "True";
  }

  void setValue(std::string& value, const std::string& input) const {
    value = input;
  }
};

#endif // !CPPDICT_CONSOLE_READ_WRITER_HPP
