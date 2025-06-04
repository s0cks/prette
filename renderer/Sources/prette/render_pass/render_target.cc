#include "prette/render_pass/render_target.h"

#include <algorithm>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/framebuffer/framebuffer.h"
#include "prette/framebuffer/framebuffer_builder.h"
#include "prette/image/image.h"
#include "prette/image/image_builder.h"
#include "prette/image/image_view.h"
#include "prette/image/image_view_builder.h"
#include "prette/render_pass/render_pass.h"
#include "prette/vk.h"
#include "prette/vk_cmd_buffers.h"

namespace prt {
static inline auto CreateImage(const VkExtent2D extent, const VkFormat format) -> vk::Image* {
  vk::ImageBuilder img_builder{};
  const auto image = img_builder.WithType(VK_IMAGE_TYPE_2D)
                         .WithFormat(format)
                         .WithExtent(extent, 1.0f)
                         .WithArrayLayers(1)
                         .WithMipLevels(1)
                         .WithLinearTiling()
                         .WithUsage(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
                         .Build();
  image->BindMemory();
  vk::SingleUseCommandBuffer buffer{};
  vk::ImageMemoryBarrier img_barrier(image);
  img_barrier.WithSourceAccessMask(VK_ACCESS_TRANSFER_READ_BIT)
      .WithDestAccessMask(VK_ACCESS_MEMORY_READ_BIT)
      .WithNewLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
      .WithSubresourceRange(VkImageSubresourceRange{
          VK_IMAGE_ASPECT_COLOR_BIT,
          0,
          1,
          0,
          1,
      });
  // clang-format off
  vk::PipelineBarrier barrier{};
  barrier.WithImageBarrier(img_barrier)
    .WithStageMask(VK_PIPELINE_STAGE_TRANSFER_BIT)
    .Apply(buffer);
  // clang-format on
  return image;
}

static inline auto CreateImageView(vk::Image* image, const VkFormat format) -> vk::ImageView* {
  ASSERT(image);
  vk::ImageViewBuilder builder{};
  // clang-format off
  return builder.WithType(VK_IMAGE_VIEW_TYPE_2D)
    .WithImage(image)
    .WithFormat(format)
    .Build();
  // clang-format on
}

static inline auto CreateFramebuffer(vk::RenderPass* pass, const VkExtent2D extent, vk::ImageView* color,
                                     vk::ImageView* depth) -> Framebuffer* {
  ASSERT_INITIALIZED(pass);
  ASSERT_INITIALIZED(color);
  // clang-format off
  FramebufferBuilder builder{};
  builder.WithRenderPass(pass)
    .WithLayers(1)
    .WithSize(extent)
    .WithAttachment(color);
  // clang-format on
  if (depth != nullptr)
    builder.WithAttachment(depth);
  return builder.Build();
}

RenderTarget::RenderTarget(const uint64_t index, vk::RenderPass* pass, const VkExtent2D& extent, const VkFormat format,
                           vk::ImageView* depth_view) :
  index_(index),
  image_(CreateImage(extent, format)),
  view_(CreateImageView(image_, format)),
  framebuffer_(CreateFramebuffer(pass, extent, view_, depth_view)) {
  ASSERT_INITIALIZED(image_);
  ASSERT_INITIALIZED(view_);
  ASSERT_INITIALIZED(framebuffer_);
}

RenderTarget::~RenderTarget() {
  delete image_;
  delete view_;
  delete framebuffer_;
}

auto RenderTarget::IsInitialized() const -> bool {
  return vk::AllInitialized(image_, view_, framebuffer_);
}

RenderTargetSet::RenderTargetSet(const uint64_t num_targets, vk::RenderPass* pass, const VkExtent2D& extent,
                                 const VkFormat format, vk::ImageView* depth_view) :
  targets_(num_targets) {
  if (num_targets > 0) {
    for (auto idx = 0; idx < num_targets; idx++) {
      targets_[idx] = new RenderTarget(idx, pass, extent, format, depth_view);
      ASSERT_INITIALIZED(targets_[idx]);
    }
  }
}

RenderTargetSet::~RenderTargetSet() {
  if (targets_.empty())
    return;
  std::ranges::for_each(targets_, [this](RenderTarget* target) {
    ASSERT_INITIALIZED(target);
    delete target;
  });
}
}  // namespace prt