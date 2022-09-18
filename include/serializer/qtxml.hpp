#ifndef CPPDICT_SERIALIZER_QT_XML_HPP
#define CPPDICT_SERIALIZER_QT_XML_HPP

#include <concepts>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "concepts.hpp"

template<typename T>
concept Unspecified = not std::integral<T> and not Detail::IsAnyOf<T, std::string, bool>;

struct QtStreamWriter {
  explicit QtStreamWriter(std::shared_ptr<QXmlStreamWriter> writer)
    : _writer(std::move(writer)) {}

 public:
  void write(std::string_view name, const auto& value) {
    _writer->writeTextElement(QString(name.data()), this->qstringValue(value));
  }

  void write(std::string_view name) {
    _writer->writeEmptyElement(QString(name.data()));
  }

  void writeValue(const auto& value) {
    _writer->writeCharacters(this->qstringValue(value));
  }

  void writeObjStartElement(std::string_view name) {
    _writer->writeStartElement(QString(name.data()));
  }

  void writeObjEndElement() {
    _writer->writeEndElement();
  }

  void writeArrayStartElement(std::string_view name) {
    this->writeObjStartElement(name);
  }

  void writeArrayEndElement() {
    this->writeObjEndElement();
  }

  void writeEntryStartElement(std::string_view name) {
    this->writeObjStartElement(name);
  }

  void writeEntryEndElement() {
    this->writeObjEndElement();
  }

  void writeAttr(std::string_view name, const auto& value) {
    _writer->writeAttribute(QString(name.data()), this->qstringValue(value));
  }

  void writeAttrStartElement() {}

  void writeAttrEndElement() {}

 private:
  QString qstringValue(std::integral auto value) const {
    return QString::number(value);
  }

  QString qstringValue(bool value) const {
    return value ? "True" : "False";
  }

  QString qstringValue(const std::string& value) const {
    return QString(value.data());
  }

 private:
  std::shared_ptr<QXmlStreamWriter> _writer;
};

struct QtStreamReader {
  explicit QtStreamReader(std::shared_ptr<QXmlStreamReader> reader)
    : _reader(std::move(reader)) {}

  [[nodiscard]] std::pair<std::string, bool> nextEntryName() const {
    const auto [nameRef, reached] = this->nextStartElement();
    if (!reached) {
      return { {}, false };
    }

    auto name = nameRef.toString().toStdString();
    return { std::move(name), true };
  }

  bool nextArrayEntry() const {
    const auto& [_, reached] = this->nextStartElement();
    return reached;
  }

  bool value(auto& value) const {
    const auto input = _reader->readElementText();

    this->setValue(value, input);
    return true;
  }

  bool attrValue(std::string_view name, auto& value) const {
    auto attrs = _reader->attributes();
    if (!attrs.hasAttribute(QString(name.data()))) {
      return false;
    }

    auto input = attrs.value(QString(name.data()));
    this->setValue(value, input.toString());
    return true;
  }

 private:
  [[nodiscard]] std::pair<QStringRef, bool> nextStartElement() const {
    const bool reached = _reader->readNextStartElement();

    return { reached ? _reader->name() : QStringRef{}, reached };
  }

  void setValue(std::integral auto& value, const QString& input) const {
    value = input.toInt();
  }

  void setValue(bool& value, const QString& input) const {
    value = input.compare("True", Qt::CaseInsensitive) == 0;
  }

  void setValue(std::string& value, const QString& input) const {
    value = input.toStdString();
  }

 private:
  std::shared_ptr<QXmlStreamReader> _reader;
};

#endif // !CPPDICT_SERIALIZER_QT_XML_HPP
