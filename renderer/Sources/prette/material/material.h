#ifndef PRT_MATERIAL_H
#define PRT_MATERIAL_H

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <utility>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/texture.h"

namespace prt {
enum MaterialComponent : uint32_t {
  kAlbedoTexture = 0,
  kNormalTexture,
  kMetallicTexture,
  kRoughnessTexture,
  kAoTexture,
  kTotalNumberOfMaterialComponents,
};

using MaterialTextureArray = std::array<Texture*, kTotalNumberOfMaterialComponents>;

class Material {
 private:
  std::string name_;
  MaterialTextureArray textures_{};
  vk::DescriptorSet* descriptors_ = nullptr;

  void UpdateDescriptorSet();

 public:
  Material(const std::string name, const MaterialTextureArray textures);
  ~Material() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetAlbedoTexture() const -> Texture* {
    return textures_.at(kAlbedoTexture);
  }

  auto GetNormalTexture() const -> Texture* {
    return textures_.at(kNormalTexture);
  }

  auto GetMetallicTexture() const -> Texture* {
    return textures_.at(kMetallicTexture);
  }

  auto GetRoughnessTexture() const -> Texture* {
    return textures_.at(kRoughnessTexture);
  }

  auto GetAoTexture() const -> Texture* {
    return textures_.at(kAoTexture);
  }

  auto GetDescriptorSet() const -> vk::DescriptorSet* {
    return descriptors_;
  }
};

class MaterialBuilder {
 private:
  std::string name_{};
  MaterialTextureArray textures_{};

 public:
  MaterialBuilder() {
    std::ranges::fill(textures_, nullptr);
  }
  ~MaterialBuilder() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto WithName(const std::string rhs) -> MaterialBuilder& {
    ASSERT_NOT_EMPTY(rhs);
    name_ = std::move(rhs);
    return *this;
  }

  auto GetMaterialTextures() const -> const MaterialTextureArray& {
    return textures_;
  }

  auto WithComponentTexture(const MaterialComponent component, Texture* texture) -> MaterialBuilder& {
    ASSERT_INITIALIZED(texture);
    textures_.at(component) = texture;
    return *this;
  }

  auto WithAlbedoTexture(Texture* rhs) -> MaterialBuilder& {
    ASSERT_INITIALIZED(rhs);
    return WithComponentTexture(kAlbedoTexture, rhs);
  }

  auto WithNormalTexture(Texture* rhs) -> MaterialBuilder& {
    ASSERT_INITIALIZED(rhs);
    return WithComponentTexture(kNormalTexture, rhs);
  }

  auto WithMetallicTexture(Texture* rhs) -> MaterialBuilder& {
    ASSERT_INITIALIZED(rhs);
    return WithComponentTexture(kMetallicTexture, rhs);
  }

  auto WithRoughnessTexture(Texture* rhs) -> MaterialBuilder& {
    ASSERT_INITIALIZED(rhs);
    return WithComponentTexture(kRoughnessTexture, rhs);
  }

  auto WithAoTexture(Texture* rhs) -> MaterialBuilder& {
    ASSERT_INITIALIZED(rhs);
    return WithComponentTexture(kAoTexture, rhs);
  }

  auto IsValid() const -> bool {
    return !name_.empty();
  }

  auto Build() -> Material* {
    ASSERT(IsValid());
    return new Material(name_, GetMaterialTextures());
  }
};
}  // namespace prt

#endif  // PRT_MATERIAL_H
