#include <catch2/catch_test_macros.hpp>
#include <solaris/framework/ecs.hpp>
#include <unordered_set>

#include "test_components.hpp"

using solaris::Entity;
using solaris::RuntimeStruct;
using solaris::World;

TEST_CASE("World create entity", "[ecs][World]") {
  World world{};
  auto shape{RuntimeStruct().withMember<ComponentA>()};

  auto [entity, object]{world.createEntity(shape)};
  REQUIRE(object != nullptr);
  auto selection{object.select<ComponentA>()};

  selection->emplaceField<ComponentA>(12);
  REQUIRE(selection->getField<ComponentA>().value == 12);
}

TEST_CASE("World view entities", "[ecs][World]") {
  World world{};
  auto shapeA{RuntimeStruct().withMember<ComponentA>()};
  auto shapeAB{shapeA.withMember<ComponentB>()};
  auto shapeABC{shapeAB.withMember<ComponentC>()};
  auto shapeAC{shapeA.withMember<ComponentC>()};

  std::unordered_set<Entity> entitiesA;
  std::unordered_set<Entity> entitiesAB;
  std::unordered_set<Entity> entitiesABC;
  std::unordered_set<Entity> entitiesAC;
  for (size_t i{0}; i < 10; ++i) {
    entitiesA.insert(world.createEntity(shapeA).first);
    entitiesAB.insert(world.createEntity(shapeAB).first);
    entitiesABC.insert(world.createEntity(shapeABC).first);
    entitiesAC.insert(world.createEntity(shapeAC).first);
  }

  auto view{World::View::withComponents<ComponentA, ComponentB>()};

  std::unordered_set<Entity> queryEntities;
  auto query{world.query(view)};
  for (auto[entity, _] : query) {
    queryEntities.insert(entity);
  }

  REQUIRE(std::ranges::all_of(entitiesAB, [&](auto entity) {
    return queryEntities.contains(entity);
  }));
  REQUIRE(std::ranges::all_of(entitiesABC, [&](auto entity) {
    return queryEntities.contains(entity);
  }));

  REQUIRE(std::ranges::none_of(entitiesA, [&](auto entity) {
    return queryEntities.contains(entity);
  }));
  REQUIRE(std::ranges::none_of( entitiesAC, [&](auto entity) {
    return queryEntities.contains(entity);
  }));
}
