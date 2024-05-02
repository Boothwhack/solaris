#include <catch2/catch_test_macros.hpp>
#include <solaris/framework/allocation.hpp>
#include <solaris/framework/runtime_object.hpp>

#include "test_components.hpp"

using solaris::Allocation;
using solaris::RuntimeStruct;
using solaris::SelectiveObjectPtr;

TEST_CASE(
    "SelectiveObjectPtr field offset",
    "[ecs][RuntimeStruct][SelectiveObjectPtr]"
) {
  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};

  Allocation objectAllocation{runtimeStruct.Size};
  SelectiveObjectPtr<ComponentA, ComponentC> obj{
      (uint8_t *)objectAllocation,
      runtimeStruct,
  };

  REQUIRE(obj.getFieldOffset<ComponentA>() == runtimeStruct.Members[0].Offset);
  REQUIRE(obj.getFieldOffset<ComponentC>() == runtimeStruct.Members[2].Offset);
}

TEST_CASE(
    "SelectiveObjectPtr field pointer",
    "[ecs][RuntimeStruct][SelectiveObjectPtr]"
) {
  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};

  Allocation objectAllocation{runtimeStruct.Size};
  SelectiveObjectPtr<ComponentA, ComponentC> obj{
      (uint8_t *)objectAllocation,
      runtimeStruct,
  };

  REQUIRE(
      obj.getFieldPtr<ComponentA>() ==
      reinterpret_cast<ComponentA *>(
          runtimeStruct.Members[0].Offset + (uint8_t *)objectAllocation
      )
  );
  REQUIRE(
      obj.getFieldPtr<ComponentC>() ==
      reinterpret_cast<ComponentC *>(
          runtimeStruct.Members[2].Offset + (uint8_t *)objectAllocation
      )
  );
}

TEST_CASE(
    "SelectiveObjectPtr emplace field",
    "[ecs][RuntimeStruct][SelectiveObjectPtr]"
) {
  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};

  Allocation objectAllocation{runtimeStruct.Size};
  SelectiveObjectPtr<ComponentA, ComponentC> obj{
      (uint8_t *)objectAllocation,
      runtimeStruct,
  };

  obj->emplaceField<ComponentA>(10);
  obj->emplaceField<ComponentC>("Hello, World!");

  REQUIRE(obj.getFieldPtr<ComponentA>()->value == 10);
  REQUIRE(obj.getFieldPtr<ComponentC>()->value == "Hello, World!");
}

TEST_CASE(
    "SelectiveObjectPtr get field reference",
    "[ecs][RuntimeStruct][SelectiveObjectPtr]"
) {
  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};

  Allocation objectAllocation{runtimeStruct.Size};
  SelectiveObjectPtr<ComponentA, ComponentC> obj{
      (uint8_t *)objectAllocation,
      runtimeStruct,
  };

  obj->emplaceField<ComponentA>(12);
  obj->emplaceField<ComponentC>("Goodbye, World!");

  REQUIRE(obj->getField<ComponentA>().value == 12);
  REQUIRE(obj->getField<ComponentC>().value == "Goodbye, World!");
}

TEST_CASE(
    "SelectiveObjectPtr as iterator",
    "[ecs][RuntimeStruct][SelectiveObjectPtr]"
) {
  auto runtimeStruct{RuntimeStruct()
                         .withMember<ComponentA>()
                         .withMember<ComponentB>()
                         .withMember<ComponentC>()};

  Allocation objectAllocation{runtimeStruct.Stride * 3};
  SelectiveObjectPtr<ComponentA, ComponentC> begin{
      (uint8_t *)objectAllocation,
      runtimeStruct,
  };
  SelectiveObjectPtr<ComponentA, ComponentC> end{
      runtimeStruct.Stride * 3 + (uint8_t *)objectAllocation,
      runtimeStruct,
  };

  {
    int index{0};
    for (auto it = begin; it != end; ++it) {
      it->emplaceField<ComponentA>(20 + index);
      it->emplaceField<ComponentC>(std::format("Hello, {}!", index));
      ++index;
    }
  }
  {
    int index{0};
    for (auto it = begin; it != end; ++it) {
      REQUIRE(it->getField<ComponentA>().value == 20 + index);
      REQUIRE(
          it->getField<ComponentC>().value == std::format("Hello, {}!", index)
      );
      ++index;
    }
  }
}
