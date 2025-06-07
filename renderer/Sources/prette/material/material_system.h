#ifndef PRT_MATERIAL_SYSTEM_H
#define PRT_MATERIAL_SYSTEM_H

#include <filesystem>
#include <string>
#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/descriptor_set_layout.h"
#include "prette/vk.h"

namespace prt {
class Material;
class MaterialSystem {
  friend class Material;
  static auto CreateMaterialDescriptorSetLayout() -> vk::DescriptorSetLayout*;

 private:
  vk::DescriptorSetLayout* material_descriptor_layout_ = nullptr;
  std::vector<fs::path> roots_{};
  std::vector<Material*> materials_{};

  void LoadDefaultMaterial();
  auto LoadMaterialFromDirectory(const fs::path dir) -> Material*;
  auto NewMaterialDescriptorSet(const std::string name) -> vk::DescriptorSet*;

 public:
  MaterialSystem();
  ~MaterialSystem();

  void AddRoot(const fs::path rhs);
  auto LoadMaterial(const std::string path) -> Material*;

  auto GetMaterial(const uint64_t idx) const -> Material* {
    ASSERT(idx >= 0 && idx <= materials_.size());
    return materials_[idx];
  }

  auto GetNumberOfMaterialsLoaded() const -> uint64_t {
    return materials_.size();
  }

  auto GetMaterialDescriptorSetLayout() const -> vk::DescriptorSetLayout* {
    return material_descriptor_layout_;
  }

 public:
  static void InitSystem();
  static auto IsSystemInitialized() -> bool;
  static auto GetSystem() -> MaterialSystem*;
};
}  // namespace prt

#endif  // PRT_MATERIAL_SYSTEM_H
