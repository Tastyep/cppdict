#ifndef CPPDICT_CONSOLE_READ_WRITER_HPP
#define CPPDICT_CONSOLE_READ_WRITER_HPP

#include <concepts>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

std::string readInput(std::string_view prefix) {
  std::string input;
  std::cout << prefix;
  std::cin >> input;

  return input;
}

struct CoutWriter {
 public:
  void write(std::string_view name, const std::integral auto value) {
    std::cout << gIndent << "'" << name << "' (integral) : " << value << std::endl;
  }

  void write(std::string_view name, bool value) {
    std::cout << gIndent << "'" << name << "' (bool) : " << value << std::endl;
  }

  void write(std::string_view name, const std::string& value) {
    std::cout << gIndent << "'" << name << "' (string) : "
              << "\"" << value << "\"" << std::endl;
  }

  void write(std::string_view name, const auto& value) {
    std::cout << gIndent << "'" << name << "' (T) : " << value << std::endl;
  }

  void write(std::string_view name) {
    std::cout << gIndent << "'" << name << "' (void)" << std::endl;
  }

  template<typename T>
  void writeValue(const T& value) {
    std::cout << gIndent << value << std::endl;
  }

  void writeObjStartElement(std::string_view name) {
    std::cout << gIndent << "'" << name << "' : {" << std::endl;
    this->addIndent(2);
  }

  void writeObjEndElement() {
    this->addIndent(-2);
    std::cout << gIndent << "}" << std::endl;
  }

  void writeArrayStartElement(std::string_view name) {
    std::cout << gIndent << "'" << name << "' : [" << std::endl;
    this->addIndent(2);
  }

  void writeArrayEndElement() {
    this->addIndent(-2);
    std::cout << gIndent << "]" << std::endl;
  }

  void addIndent(int amount) {
    int indent = gIndent.size() + amount;
    gIndent = std::string(indent, ' ');
  }

  std::string gIndent;
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
    const auto& [input, eof] = this->nextEntryName();
    if (eof) {
      return false;
    }

    this->setValue(value, input);
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
