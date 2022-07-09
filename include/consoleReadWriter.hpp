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

  void writeStartElement(std::string_view name) {
    std::cout << gIndent << "'" << name << "' : {" << std::endl;
    this->addIndent(2);
  }

  void writeEndElement() {
    this->addIndent(-2);
    std::cout << gIndent << "}" << std::endl;
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
    return { std::move(str), std::cin.eof() };
  }

  void value(std::integral auto& value) const {
    value = std::stoi(readInput(prefix));
  }

  void value(bool& value) const {
    value = readInput(prefix) == "True";
  }

  void value(std::string& value) const {
    value = readInput(prefix);
  }
};

#endif // !CPPDICT_CONSOLE_READ_WRITER_HPP
