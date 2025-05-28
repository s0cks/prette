#ifndef PRT_DESCRIPTOR_SET_UPDATE_H
#define PRT_DESCRIPTOR_SET_UPDATE_H

#include <vector>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/vk.h"

namespace prt {
class Camera;
}

namespace prt::vk {
class DescriptorSetUpdate {
 public:
  using Write = VkWriteDescriptorSet;
  using WriteList = std::vector<Write>;

  class WriteBuilder {
    friend class DescriptorSetUpdate;
    DEFINE_DEFAULT_COPYABLE_TYPE(WriteBuilder);

   private:
    Write* write_;

    explicit WriteBuilder(Write* w, VkDescriptorSet dset, const uint32_t binding, const VkDescriptorType type) :
      write_(w) {
      ASSERT(write_);
      write()->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
      write()->dstSet = dset;
      write()->dstBinding = binding;
      write()->dstArrayElement = 0;
      write()->descriptorCount = 0;
      write()->descriptorType = type;
      write()->pBufferInfo = nullptr;
      write()->pImageInfo = nullptr;
      write()->pTexelBufferView = nullptr;
    }

   public:
    ~WriteBuilder() = default;

    auto write() const -> Write* {
      return write_;
    }

    auto WithBufferInfo(const VkDescriptorBufferInfo* rhs) -> WriteBuilder& {
      write_->pBufferInfo = rhs;
      write_->descriptorCount = 1;
      return *this;
    }

    auto WithBufferInfo(Buffer* rhs) -> WriteBuilder&;
    auto WithCameraBuffer(Camera* rhs) -> WriteBuilder&;
  };

 private:
  VkDescriptorSet dset_;
  WriteList writes_{};

  inline auto writes() const -> const VkWriteDescriptorSet* {
    return writes_.data();
  }

 public:
  DescriptorSetUpdate(VkDescriptorSet dset, const WriteList writes = {});
  explicit DescriptorSetUpdate(vk::DescriptorSet* dset);
  ~DescriptorSetUpdate();

  auto GetWrites() const -> const WriteList& {
    return writes_;
  }

  auto GetNumberOfWrites() const -> uint64_t {
    return writes_.size();
  }

  auto IsEmpty() const -> bool {
    return writes_.empty();
  }

  auto AddWrite(const uint32_t binding, const VkDescriptorType type) -> WriteBuilder {
    const auto idx = GetNumberOfWrites();
    writes_.resize(idx + 1);
    return WriteBuilder(&writes_[idx], dset_, binding, type);
  }

  inline auto AddWriteUniformBuffer(const uint32_t binding) -> WriteBuilder {
    return AddWrite(binding, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
  }

  inline auto AddWriteCameraUniformBuffer(const uint32_t binding, Camera* camera) -> WriteBuilder {
    ASSERT(camera);
    return AddWriteUniformBuffer(binding).WithCameraBuffer(camera);
  }

  inline auto AddWriteSampler(const uint32_t binding) -> WriteBuilder {
    return AddWrite(binding, VK_DESCRIPTOR_TYPE_SAMPLER);
  }

  void Commit();
};
}  // namespace prt::vk

#endif  // PRT_DESCRIPTOR_SET_UPDATE_H
