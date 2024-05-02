#pragma once

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <ranges>
#include <set>
#include <solaris/framework/runtime_vector.hpp>
#include <unordered_map>
#include <unordered_set>
#include <utility>

namespace solaris {
using Entity = size_t;

class Archetype {
  RuntimeVector m_Storage;
  std::vector<Entity> m_Entities;

public:
  Archetype(RuntimeStruct runtimeStruct)
      : m_Storage{std::move(runtimeStruct)} {}

  Archetype() : m_Storage{RuntimeStruct()} {}

  const RuntimeStruct &runtimeStruct() const {
    return m_Storage.runtimeStruct();
  }

  const RuntimeVector &storage() const { return m_Storage; }

  const std::vector<Entity> &entities() const { return m_Entities; }

  Archetype withComponent(RuntimeField field) const {
    return {m_Storage.runtimeStruct().withField(field)};
  }

  Archetype withComponents(const std::ranges::range auto &fields) const {
    RuntimeStruct newRuntimeStruct{m_Storage.runtimeStruct()};
    for (const RuntimeField &field : fields) {
      newRuntimeStruct = newRuntimeStruct.withField(field);
    }
    return newRuntimeStruct;
  }

  [[nodiscard]] RawObjectPtr get(size_t index) const {
    return m_Storage[index];
  }

  std::pair<size_t, RawObjectPtr> add(Entity entity) {
    auto index{m_Entities.size()};
    auto obj{m_Storage.pushBack()};
    m_Entities.push_back(entity);

    return {index, obj};
  }
};

class World {
  struct AliveEntity {
    size_t ArchetypeID;
    size_t Index;
  };

  Entity m_NextEntity{0};
  std::vector<Archetype> m_Archetypes;
  std::unordered_map<Entity, AliveEntity> m_Entities;

public:
  template <typename... Cs>
  class SelectiveView {
    std::set<std::type_index> m_Types;

  public:
    SelectiveView() { m_Types.insert({typeid(Cs)...}); }

    bool matchesArchetype(const Archetype &archetype) const {
      using std::cout;
      using std::endl;

      const auto &shape{archetype.runtimeStruct()};

      cout << "Considering archetype:" << endl;
      for (const auto &member : shape.Members)
        cout << "  " << member.Field.TypeIndex.name() << endl;

      auto matches{std::ranges::all_of(
          m_Types,
          [&](const std::type_index &type) {
            return std::ranges::contains(
                shape.Members,
                type,
                [](const RuntimeStruct::Member &member) {
                  return member.Field.TypeIndex;
                }
            );
          }
      )};

      cout << "    Matches? " << matches << endl;
      return matches;
    }

    auto viewArchetype(const Archetype &archetype) const {
      static_assert(std::ranges::viewable_range<RuntimeVector::View<Cs...>>);
      static_assert(std::ranges::viewable_range<const std::vector<Entity> &>);
      return std::ranges::views::zip_transform(
          [](Entity entity, SelectiveObjectRef<Cs...> obj) {
            return QueryResult<Cs...>(entity, obj);
          },
          archetype.entities(),
          RuntimeVector::View<Cs...>{archetype.storage()}
      );
    }
  };

  class View {
  public:
    template <typename... Cs>
    static SelectiveView<Cs...> withComponents() {
      return SelectiveView<Cs...>{};
    }
  };

  template <typename... Ts>
  struct QueryResult {
    Entity EntityID;
    SelectiveObjectRef<Ts...> Components;
  };

private:
  std::pair<size_t, Archetype &>
  findOrAddArchetype(const RuntimeStruct &requirements) {
    std::unordered_set<std::type_index> requiredFields;
    for (const RuntimeStruct::Member &member : requirements.Members)
      requiredFields.insert(member.Field.TypeIndex);

    // try to find an archetype with exactly the same members
    for (size_t index = 0; index < m_Archetypes.size(); ++index) {
      auto &archetype{m_Archetypes[index]};
      const auto &members{archetype.runtimeStruct().Members};
      if (members.size() != requirements.Members.size())
        continue;
      if (std::ranges::all_of(members, [&](auto &member) {
            return requiredFields.contains(member.Field.TypeIndex);
          }))
        return {index, archetype};
    }

    size_t index{m_Archetypes.size()};
    auto &archetype{m_Archetypes.emplace_back(requirements)};

    return {index, archetype};
  }

public:
  std::pair<Entity, RawObjectPtr> createEntity(const RuntimeStruct &shape) {
    auto entity{++m_NextEntity};

    std::unordered_set<std::type_index> fields;
    for (const RuntimeStruct::Member &member : shape.Members)
      fields.insert(member.Field.TypeIndex);

    auto [id, archetype] = findOrAddArchetype(shape);
    auto [index, obj] = archetype.add(entity);

    m_Entities[entity] = AliveEntity{.ArchetypeID = id, .Index = index};

    return {entity, obj};
  }

  RawObjectPtr getEntity(Entity id) const {
    auto it{m_Entities.find(id)};
    if (it == m_Entities.end()) {
      return nullptr;
    }

    const auto &archetype{m_Archetypes[it->second.ArchetypeID]};
    return archetype.get(it->second.Index);
  }

  template <typename V>
  auto query(const V &view) const {
    return m_Archetypes |
           std::ranges::views::filter([&](const Archetype &archetype) {
             return view.matchesArchetype(archetype);
           }) |
           std::ranges::views::transform([&](const Archetype &archetype) {
             return view.viewArchetype(archetype);
           }) |
           std::ranges::views::join;
  }
};
} // namespace solaris
