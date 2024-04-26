#include <catch2/catch_test_macros.hpp>
#include <solaris/framework/runtime_struct.hpp>

#include "test_components.hpp"

using solaris::RuntimeField;
using solaris::RuntimeStruct;

TEST_CASE("RuntimeField attributes", "[ecs][RuntimeStruct][RuntimeField]") {
  auto field{RuntimeField::runtimeFieldFor<Moveable>()};

  REQUIRE(field.Size == sizeof(Moveable));
  REQUIRE(field.Alignment == alignof(Moveable));
}

TEST_CASE("RuntimeField copy", "[ecs][RuntimeStruct][RuntimeField]") {
  auto field{RuntimeField::runtimeFieldFor<Moveable>()};

  Moveable source{10};
  Moveable destination{0};

  REQUIRE(source.value == 10);
  REQUIRE(destination.value == 0);

  field.CopyFunction(
      reinterpret_cast<void *>(&source),
      reinterpret_cast<void *>(&destination)
  );

  REQUIRE(source.value == 10);
  REQUIRE(destination.value == 10);
}

TEST_CASE("RuntimeField destruct", "[ecs][RuntimeStruct][RuntimeField]") {
  auto field{RuntimeField::runtimeFieldFor<Moveable>()};

  Moveable value{10};

  REQUIRE(value.value == 10);

  field.DestructorFunction(reinterpret_cast<void *>(&value));

  REQUIRE(value.value == 0);
}

TEST_CASE("RuntimeField move", "[ecs][RuntimeStruct]") {
  auto field{RuntimeField::runtimeFieldFor<Moveable>()};

  Moveable source{10};
  Moveable destination{0};

  REQUIRE(source.value == 10);
  REQUIRE(destination.value == 0);

  field.MoveFunction(
      reinterpret_cast<void *>(&source),
      reinterpret_cast<void *>(&destination)
  );

  REQUIRE(source.value == 0);
  REQUIRE(destination.value == 10);
}

TEST_CASE("RuntimeStruct withMember", "[ecs][RuntimeStruct]") {
  auto runtimeStruct{RuntimeStruct().withMember<ComponentA>()};

  REQUIRE(runtimeStruct.Size == sizeof(ComponentA));
  REQUIRE(runtimeStruct.Stride == sizeof(ComponentA));
  REQUIRE(runtimeStruct.Alignment == alignof(ComponentA));
  REQUIRE(runtimeStruct.Members.size() == 1);
  REQUIRE(runtimeStruct.Members[0].Offset == 0);
  REQUIRE(runtimeStruct.Members[0].Field.TypeIndex == typeid(ComponentA));

  auto withB{runtimeStruct.withMember<ComponentB>()};

  // should not modify base runtime struct
  REQUIRE(runtimeStruct.Members.size() == 1);

  REQUIRE(withB.Members.size() == 2);
  REQUIRE(withB.Members[0].Field.TypeIndex == typeid(ComponentA));
  REQUIRE(withB.Members[1].Field.TypeIndex == typeid(ComponentB));
  REQUIRE(withB.Members[1].Offset > withB.Members[0].Offset);
  REQUIRE(withB.Members[1].Offset < withB.Size);
  REQUIRE(withB.Stride >= withB.Size);
}
