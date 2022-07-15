#include <functional>

#include "consoleReadWriter.hpp"
#include "deserializer.hpp"
#include "serializer.hpp"

template<typename T>
using ref = std::reference_wrapper<T>;

class NamedData {
 public:
  static constexpr auto EntryName = "OtherData";

  void serialize(auto& serializer) const {
    serializer.serialize(Entry("Int", _value), //
                         Entry("Str", _name));
  }

  void deserialize(auto& deserializer) {
    using std::ref;
    deserializer.deserialize(Entry("Int", ref(_value)), //
                             Entry("Str", ref(_name)));
  }

 private:
  int _value{ 42 };
  std::string _name{ EntryName };
};

class Data {
 public:
  void serialize(auto& serializer) const {
    serializer.serialize(Entry("Int", _value), //
                         Entry("Str", _name),
                         Entry("DataVec", _otherData));
  }

  void deserialize(auto& deserializer) {
    using std::ref;
    deserializer.deserialize(Entry("Int", ref(_value)), //
                             Entry("Str", ref(_name)),
                             Entry("DataVec", ref(_otherData)));
  }

 private:
  int _value{ 42 };
  std::string _name{ "Data" };
  std::vector<NamedData> _otherData;
};

int main() {
  Serializer serializer(CoutWriter{});
  Deserializer deserializer(CinReader{});

  auto tree = Entry("Root", //
                    std::tuple{
                      Entry("Int", 5),
                      Entry("Str", "Test"),
                      Entry("Data", Data{}),
                      Entry("Vec", std::vector<int>{}),
                      Entry("Child",
                            std::tuple{
                              Entry("Bool", true),
                            }),
                    });
  auto otherTree = Entry("Root",
                         std::tuple{
                           Entry("int", 6),
                         });

  // Read only
  std::cout << *tree.get<int>("Root.Int") << std::endl;

  // Read / Write
  tree.get<ref<int>>("Root.Int")->get() = 21;

  serializer.serialize(tree);

  std::cout << '\n' << "Enter entry name:" << std::endl;
  deserializer.deserialize(tree);

  std::cout << '\n' << "Result:" << std::endl;
  serializer.serialize(tree);
  return 0;
}
