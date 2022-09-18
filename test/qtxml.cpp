#include "./data.hpp"

#include "deserializer.hpp"
#include "serializer.hpp"
#include "serializer/qtxml.hpp"

#include <QBuffer>
#include <QString>
#include <memory>

int main(void) {
  QBuffer buffer;
  buffer.open(QBuffer::ReadWrite);

  auto streamReader = std::make_shared<QXmlStreamReader>();
  auto streamWriter = std::make_shared<QXmlStreamWriter>(&buffer);
  streamWriter->setAutoFormatting(true);

  QtStreamWriter writer(streamWriter);
  Serializer serializer(std::move(writer));

  QtStreamReader reader(streamReader);
  Deserializer deserializer(std::move(reader));

  auto tree = make_data();

  std::cout << "[qtxml] Serialization:" << std::endl;
  serializer.serialize(tree);
  std::cout << QString(buffer.data()).toStdString() << std::endl;

  std::cout << '\n' << "[qtxml] Deserialization" << std::endl;
  streamReader->addData(buffer.data());
  deserializer.deserialize(tree);

  std::cout << '\n' << "[qtxml] Serialization:" << std::endl;
  buffer.reset();
  serializer.serialize(tree);
  std::cout << QString(buffer.data()).toStdString() << std::endl;
}
