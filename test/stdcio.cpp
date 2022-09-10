#include "./data.hpp"

#include "deserializer.hpp"
#include "serializer.hpp"
#include "serializer/stdcio.hpp"

int main(void) {
  Serializer serializer(StdCout{});
  Deserializer deserializer(StdCin{});

  auto tree = make_data();

  auto& val = tree.get<"Root/Child/Bool">();
  val = false;

  auto& attr = tree.get<"Root/Int/@ENABLE">();
  attr = false;

  std::cout << '\n' << "Serialization:" << std::endl;
  serializer.serialize(tree);
  std::cout << '\n' << "Deserialization:" << std::endl;
  deserializer.deserialize(tree);

  std::cout << '\n' << "Serialization:" << std::endl;
  serializer.serialize(tree);
}
