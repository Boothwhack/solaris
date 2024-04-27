#include <catch2/catch_test_macros.hpp>
#include <solaris/framework/runtime_vector.hpp>

#include "test_components.hpp"

using solaris::RuntimeStruct;
using solaris::RuntimeVector;
using solaris::SelectiveObjectPtr;

TEST_CASE("RuntimeVector push back", "[ecs][RuntimeVector]") {
  using solaris::RuntimeStruct;
  using solaris::RuntimeVector;

  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};
  RuntimeVector vector{runtimeStruct};
  {
    auto element{vector.pushBack()};
    auto obj{element.select<ComponentA, ComponentB, ComponentC>()};
    obj.emplaceField<ComponentA>(16);
    obj.emplaceField<ComponentB>(1.5);
    obj.emplaceField<ComponentC>("Hello, World!");
  }
  REQUIRE(vector.size() == 1);
  REQUIRE(vector.capacity() >= 1);
}

TEST_CASE("RuntimeVector get", "[ecs][RuntimeVector]") {
  using solaris::RuntimeStruct;
  using solaris::RuntimeVector;
  using solaris::SelectiveObjectPtr;

  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};
  RuntimeVector vector{runtimeStruct};
  {
    auto element{vector.pushBack()};
    auto obj{element.select<ComponentA, ComponentB, ComponentC>()};
    obj.emplaceField<ComponentA>(16);
    obj.emplaceField<ComponentB>(1.5);
    obj.emplaceField<ComponentC>("Hello, World!");
  }

  {
    auto obj{vector[0].select<ComponentA, ComponentC>()};
    REQUIRE(obj.getField<ComponentA>().value == 16);
    REQUIRE(obj.getField<ComponentC>().value == "Hello, World!");
  }
}

TEST_CASE("RuntimeVector dynamic allocation", "[ecs][RuntimeVector]") {
  using solaris::RuntimeStruct;
  using solaris::RuntimeVector;

  auto runtimeStruct{RuntimeStruct().withMember<ComponentA>()};
  RuntimeVector vector{runtimeStruct};
  {
    auto element{vector.pushBack()};
    auto obj{element.select<ComponentA>()};
    obj.emplaceField<ComponentA>(16);
  }

  auto capacity{vector.capacity()};
  for (size_t i{0}; i < capacity; ++i) {
    auto element{vector.pushBack()};
    auto obj{element.select<ComponentA>()};
    obj.emplaceField<ComponentA>(17 + i);
  }

  REQUIRE(vector.size() == capacity + 1);
  REQUIRE(vector.capacity() > capacity);

  for (size_t i{1}; i < capacity; ++i) {
    auto obj{vector[i].select<ComponentA>()};
    REQUIRE(obj.getField<ComponentA>().value == 16 + i);
  }
}
