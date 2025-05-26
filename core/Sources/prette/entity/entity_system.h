#ifndef PRT_ENTITY_SYSTEM_H
#define PRT_ENTITY_SYSTEM_H

namespace prt {
class EntitySystem {
 public:
  static constexpr const auto kSystemName = "Entity";

 public:
  EntitySystem();
  ~EntitySystem();

 public:
  static void InitSystem();
  static auto IsSystemInitialized() -> bool;
  static auto GetSystem() -> EntitySystem*;
};

/*
e -> id, tags, components
*/
}  // namespace prt

#endif  // PRT_ENTITY_SYSTEM_H
