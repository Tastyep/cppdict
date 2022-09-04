#include <functional>

#include "attr.hpp"
#include "consoleReadWriter.hpp"
#include "deserializer.hpp"
#include "entry.hpp"
#include "serializer.hpp"

template<typename T>
using ref = std::reference_wrapper<T>;

class NamedData {
 public:
  static constexpr auto EntryName = "OtherData";

  void serialize(auto& serializer) const {
    serializer.serialize(makeEntry<"Int">(_value), //
                         makeEntry<"Str">(_name));
  }

  void deserialize(auto& deserializer) {
    using std::ref;
    deserializer.deserialize(makeEntry<"Int">(ref(_value)), //
                             makeEntry<"Str">(ref(_name)));
  }

 private:
  int _value{ 42 };
  std::string _name{ EntryName };
};

class Data {
 public:
  void serialize(auto& serializer) const {
    serializer.serialize(makeEntry<"Int">(_value), //
                         makeEntry<"Str">(_name),
                         makeEntry<"DataVec">(_otherData));
  }

  void deserialize(auto& deserializer) {
    using std::ref;
    deserializer.deserialize(makeEntry<"Int">(ref(_value)), //
                             makeEntry<"Str">(ref(_name)),
                             makeEntry<"DataVec">(ref(_otherData)));
  }

 private:
  int _value{ 42 };
  std::string _name{ "Data" };
  std::vector<NamedData> _otherData;
};

int main() {
  Serializer serializer(CoutWriter{});
  Deserializer deserializer(CinReader{});

  auto tree = makeEntry<"Root">(std::tuple{
    makeEntry<"Int">(5, makeAttr<"ENABLE">(true)),
    makeEntry<"Str">("Test"),
    makeEntry<"Data">(Data{}),
    makeEntry<"Vec">(std::vector<int>{}),
    makeEntry<"Child">(std::tuple{
      makeEntry<"Bool">(true, makeAttr<"TEST">(15)),
    }),
  });

  auto& val = tree.get<"Root/Child/Bool">();
  val = false;

  auto& attr = tree.get<"Root/Int/@ENABLE">();
  attr = false;

  serializer.serialize(tree);
  std::cout << '\n' << "Enter entry name:" << std::endl;
  deserializer.deserialize(tree);

  std::cout << '\n' << "Result:" << std::endl;
  serializer.serialize(tree);

  return 0;
}
