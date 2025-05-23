#include "prette/swapchain_frame.h"

#include <string>
#include <vector>

#include "prette/common.h"
#include "prette/device.h"
#include "prette/image.h"  // IWYU pragma: keep
#include "prette/platform.h"
#include "prette/renderer.h"
#include "prette/renderer_event.h"
#include "prette/swapchain.h"
#include "prette/to_string.h"
#include "prette/vk.h"
#include "prette/vk_queue.h"

namespace prt {
SwapchainFrame::SwapchainFrame(Swapchain* owner, const uint32_t frame) :
  owner_(owner),
  frame_(frame) {
  ASSERT(owner_);
  const auto driver = Driver::Get();
  driver->CreateSemaphore(available_);
  driver->CreateSemaphore(finished_);
  driver->CreateFence(fence_, VK_FENCE_CREATE_SIGNALED_BIT);
}

SwapchainFrame::~SwapchainFrame() {
  const auto driver = Driver::Get();
  driver->DestroySemaphore(finished_);
  driver->DestroySemaphore(available_);
  driver->DestroyFence(fence_);
}

auto SwapchainFrame::GetView() const -> vk::ImageView* {
  return GetOwner()->GetView(GetImage());
}

auto SwapchainFrame::GetFramebuffer() const -> vk::Framebuffer* {
  return GetOwner()->GetFramebuffer(GetImage());
}

auto SwapchainFrame::AcquireNextImage(const uint64_t timeout, const bool waitAll) -> vk::Result {
  const auto driver = Driver::Get();
  vkWaitForFences(*driver->GetDevice(), 1, &fence_, waitAll, timeout);
  vk::Result status =
      vkAcquireNextImageKHR(*driver->GetDevice(), *GetSwapchain(), timeout, available_, VK_NULL_HANDLE, &image_);
  LOG_IF(FATAL, !status) << "failed to acquire next swapchain image: " << status;
  vkResetFences(*driver->GetDevice(), 1, &fence_);
  return status;
}

auto SwapchainFrame::ToString() const -> std::string {
  ToStringHelper<SwapchainFrame> helper{};
  helper.AddFieldRef("frame", GetFrame());
  helper.AddFieldRef("image", GetImage());
  return helper;
}

SwapchainFrameRingBuffer::SwapchainFrameRingBuffer(Swapchain* owner, const uint64_t num_frames) :
  owner_(owner),
  frames_() {
  ASSERT(owner_);
  frames_.resize(num_frames);
  for (auto idx = 0; idx < num_frames; idx++)
    frames_.at(idx) = new SwapchainFrame(owner, idx);
}

SwapchainFrameRingBuffer::~SwapchainFrameRingBuffer() {
  for (const auto& frame : frames_)
    delete frame;
}

SwapchainFrameScope::SwapchainFrameScope() {
  Renderer::Publish<PreFrameEvent>();
  frame_ = AcquireNextImage();  // NOLINT(cppcoreguidelines-prefer-member-initializer)
  ASSERT(frame_);
}

SwapchainFrameScope::~SwapchainFrameScope() {
  if (frame_ && frame_->HasCommandBuffers()) {
    if (ShouldSubmit())
      Submit();
    if (ShouldPresent())
      Present();
  }
  Renderer::Publish<PostFrameEvent>();
}

auto SwapchainFrameScope::AcquireNextImage(const uint64_t timeout, const bool waitAll) -> SwapchainFrame* {
  auto frame = GetSwapchain()->GetNextFrame();
  const auto status = frame->AcquireNextImage(timeout, waitAll);
  LOG_IF(FATAL, !status.IsSuccess() && !status.IsSubOptimal()) << "failed to acquire next swapchain image: " << status;
  if (status.IsOutOfDate()) {
    SwapchainInitializer::ReInit();
    return nullptr;
  }
  return frame;
}

void SwapchainFrameScope::Present() {
  vk::QueuePresent present{};
  const auto swap = GetSwapchain();
  // clang-format off
  present.WithImageIndex(frame_->GetImage())
    .WithSwap(swap)
    .WithWaitSemaphore(frame_->GetFinishedSemaphore());
  // clang-format on
  const auto result = present.Present();
  if (result.IsSubOptimal() || result.IsOutOfDate() || GetSwapchain()->IsResized()) {
    SwapchainInitializer::ReInit();
    return;
  }
}

void SwapchainFrameScope::Submit() {
  vk::QueueSubmit submit{};
  submit.WithWaitStageMask(VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT)
      .WithWaitSemaphore(frame_->GetAvailableSemaphore())
      .WithSignalSemaphore(frame_->GetFinishedSemaphore())
      .WithCommandBuffers(frame_->GetComamndBuffers());
  const auto status = submit.Submit(frame_->GetFence());
  LOG_IF(FATAL, !status) << "failed to submit swapchain frame: " << status;
}
}  // namespace prt