#include <functional>

#include "consoleReadWriter.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

int main() {
  Serializer serializer(CoutWriter{});
  Deserializer deserializer(CinReader{});

  auto tree = Entry("Root", //
                    std::tuple{
                      Entry("Int", 5),
                      Entry("Str", "Test"),
                      Entry("Vec", std::vector<int>{}),
                      Entry("Child",
                            std::tuple{
                              Entry("Bool", true),
                            }),
                    });

  serializer.serialize(tree);

  std::cout << '\n' << "Enter entry name:" << std::endl;
  deserializer.deserialize(tree);

  std::cout << '\n' << "Result:" << std::endl;
  serializer.serialize(tree);
  return 0;
}
