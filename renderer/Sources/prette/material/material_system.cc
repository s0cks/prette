#include "prette/material/material_system.h"

#include <filesystem>
#include <re2/re2.h>
#include <string>
#include <utility>
#include <vulkan/vulkan_core.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/descriptor_set_layout.h"
#include "prette/flags.h"
#include "prette/material/material.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/texture.h"
#include "prette/thread_local.h"

namespace prt {
static ThreadLocal<MaterialSystem> system_{};

auto MaterialSystem::CreateMaterialDescriptorSetLayout() -> vk::DescriptorSetLayout* {
  vk::DescriptorSetLayoutBuilder builder{};
  builder.AddCombinedImageSamplerBinding().WithStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT);  // albedo
  builder.AddCombinedImageSamplerBinding().WithStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT);  // normal
  builder.AddCombinedImageSamplerBinding().WithStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT);  // metallic
  builder.AddCombinedImageSamplerBinding().WithStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT);  // roughness
  builder.AddCombinedImageSamplerBinding().WithStageFlags(VK_SHADER_STAGE_FRAGMENT_BIT);  // ao
  return builder;
}

void MaterialSystem::LoadDefaultMaterial() {
  const auto material = LoadMaterial("fabrics/leather_black");
  ASSERT(material);
  DLOG(INFO) << material->GetName() << " loaded!";
  materials_.push_back(material);
}

MaterialSystem::MaterialSystem() {
  AddRoot(fs::path(FLAGS_resources) / "materials");
  OnInitDescriptorSets([this](InitDescriptorSetsEvent* event) {
    material_descriptor_layout_ = CreateMaterialDescriptorSetLayout();
    ASSERT_INITIALIZED(material_descriptor_layout_);
  });
  OnInitMaterials([this](InitMaterialsEvent* event) {
    LoadDefaultMaterial();
    {
      const auto material = LoadMaterial("grass/stylized");
      ASSERT(material);
      DLOG(INFO) << material->GetName() << " loaded!";
      materials_.push_back(material);
    }

    {
      const auto material = LoadMaterial("fabrics/leather_black");
      ASSERT(material);
      DLOG(INFO) << material->GetName() << " loaded!";
      materials_.push_back(material);
    }

    {
      const auto material = LoadMaterial("floors/old_wood");
      ASSERT(material);
      DLOG(INFO) << material->GetName() << " loaded!";
      materials_.push_back(material);
    }
  });
}

MaterialSystem::~MaterialSystem() {
  delete material_descriptor_layout_;
}

void MaterialSystem::AddRoot(const fs::path rhs) {
  if (!fs::exists(rhs) || !fs::is_directory(rhs)) {
    LOG(WARNING) << "cannot add Material root directory: " << rhs;
    return;
  }
  roots_.push_back(rhs);
}

static inline auto LoadTexture(const fs::path& path, const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB,
                               const bool flip = true, const bool staging = true) -> Texture* {
  TextureBuilder builder{};
  const auto data = ReadTexture(path, flip);
  builder.WithTextureData(data, format, staging);
  return builder;
}

auto MaterialSystem::LoadMaterialFromDirectory(const fs::path dir) -> Material* {
  static const RE2 kAlbedoPattern("albedo\\.(png|jpeg|jpg)$");
  static const RE2 kNormalPattern("normal\\.(png|jpeg|jpg)$");
  static const RE2 kMetallicPattern("metallic\\.(png|jpeg|jpg)$");
  static const RE2 kRoughnessPattern("roughness\\.(png|jpeg|jpg)$");
  static const RE2 kAoPattern("ao\\.(png|jpeg|jpg)$");

  ASSERT(fs::exists(dir) && fs::is_directory(dir));
  const auto material_name = dir.filename();
  LOG(INFO) << "loading `" << material_name << "` material....";
  MaterialBuilder builder{};
  builder.WithName(material_name);
  for (const auto& entry : fs::directory_iterator(dir)) {
    const auto& path = entry.path();
    const auto filename = path.filename().string();
    if (RE2::FullMatch(filename, kAlbedoPattern)) {
      DLOG(INFO) << "loading albedo texture " << filename << " from: " << path;
      builder.WithAlbedoTexture(LoadTexture(path));
    } else if (RE2::FullMatch(filename, kNormalPattern)) {
      DLOG(INFO) << "loading normal texture " << filename << " from: " << path;
      builder.WithNormalTexture(LoadTexture(path));
    } else if (RE2::FullMatch(filename, kMetallicPattern)) {
      DLOG(INFO) << "loading metallic texture " << filename << " from: " << path;
      builder.WithMetallicTexture(LoadTexture(path));
    } else if (RE2::FullMatch(filename, kRoughnessPattern)) {
      DLOG(INFO) << "loading roughness texture " << filename << " from: " << path;
      builder.WithRoughnessTexture(LoadTexture(path));
    } else if (RE2::FullMatch(filename, kAoPattern)) {
      DLOG(INFO) << "loading ao texture " << filename << " from: " << path;
      builder.WithAoTexture(LoadTexture(path));
    }
  }
  return builder.Build();
}

auto MaterialSystem::LoadMaterial(const std::string path) -> Material* {
  ASSERT_NOT_EMPTY(path);
  for (const auto& root : roots_) {
    const auto material_dir = root / path;
    if (!fs::exists(material_dir) || !fs::is_directory(material_dir)) {
      DLOG(WARNING) << "failed to find material: " << material_dir;
      continue;
    }
    return LoadMaterialFromDirectory(material_dir);
  }
  LOG(ERROR) << "failed to load Material named `" << path << "` in the following directories:";
  for (const auto& root : roots_)
    LOG(ERROR) << " - " << root;
  return nullptr;
}

auto MaterialSystem::NewMaterialDescriptorSet(const std::string name) -> vk::DescriptorSet* {
  return new vk::DescriptorSet(std::move(name), *GetMaterialDescriptorSetLayout());
}

void MaterialSystem::InitSystem() {
  ASSERT(!IsSystemInitialized());
  system_ = new MaterialSystem();
}

auto MaterialSystem::IsSystemInitialized() -> bool {
  return system_.Get() != nullptr;
}

auto MaterialSystem::GetSystem() -> MaterialSystem* {
  ASSERT(IsSystemInitialized());
  return system_;
}
}  // namespace prt