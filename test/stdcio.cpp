#include "./data.hpp"

#include "deserializer.hpp"
#include "serializer.hpp"
#include "serializer/stdcio.hpp"

int main(void) {
  Serializer serializer(StdCout{});
  Deserializer deserializer(StdCin{});

  auto tree = make_data();

  std::cout << "[stdcio] Serialization:" << std::endl;
  serializer.serialize(tree);
  std::cout << '\n' << "[stdcio] Deserialization:" << std::endl;
  deserializer.deserialize(tree);

  std::cout << '\n' << "[stdcio] Serialization:" << std::endl;
  serializer.serialize(tree);
}
