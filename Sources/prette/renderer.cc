#include "prette/renderer.h"

#include "prette/command_pool.h"
#include "prette/gfx.h"
#include "prette/relaxed_atomic.h"
#include "prette/swap_chain.h"
#include "prette/window.h"

namespace prt {
static std::array<VkSemaphore, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> available_semaphores_{};
static std::array<VkSemaphore, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> finished_semaphores_{};
static std::array<VkFence, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> fences_{};
static RelaxedAtomic<uint32_t> current_frame_(0);
static RelaxedAtomic<bool> resized_(false);

void Renderer::InitResizeListener() {
  const auto window = GetAppWindow();
  ASSERT(window);
  window->OnWindowSizeEvent().subscribe([](WindowSizeEvent* event) {
    ASSERT(event);
    resized_ = true;
  });
}

void Renderer::InitSyncObjects(Driver* driver) {
  ASSERT(driver);
  VkSemaphoreCreateInfo semaphore_info{};
  semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_info{};
  fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    CHECK_VK(FATAL,
             vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &available_semaphores_.at(idx)),
             "failed to create available semaphore");
    CHECK_VK(FATAL,
             vkCreateSemaphore(driver->GetDevice(), &semaphore_info, driver->GetAllocator(), &finished_semaphores_.at(idx)),
             "failed to create finished semaphore");
    CHECK_VK(FATAL, vkCreateFence(driver->GetDevice(), &fence_info, driver->GetAllocator(), &fences_.at(idx)),
             "failed to create fence");
  }
}

void Renderer::DestroySyncObjects(Driver* driver) {
  ASSERT(driver);
  for (auto idx = 0; idx < MAX_NUMBER_OF_FRAMES_IN_FLIGHT; idx++) {
    vkDestroySemaphore(driver->GetDevice(), finished_semaphores_.at(idx), driver->GetAllocator());
    vkDestroySemaphore(driver->GetDevice(), available_semaphores_.at(idx), driver->GetAllocator());
    vkDestroyFence(driver->GetDevice(), fences_.at(idx), driver->GetAllocator());
  }
}

static inline auto GetCurrentFrame() -> uint32_t {
  return (uint32_t)current_frame_;
}

static inline auto GetCurrentFence() -> const VkFence& {
  return fences_.at(GetCurrentFrame());
}

static inline auto GetCurrentAvailableSemaphore() -> const VkSemaphore& {
  return available_semaphores_.at(GetCurrentFrame());
}

static inline auto GetCurrentFinishedSemaphore() -> const VkSemaphore& {
  return finished_semaphores_.at(GetCurrentFrame());
}

void Renderer::DrawFrame(Driver* driver) {
  ASSERT(driver);
  vkWaitForFences(driver->GetDevice(), 1, &GetCurrentFence(), VK_TRUE, UINT64_MAX);
  uint32_t image_index = 0;
  {
    const auto result = vkAcquireNextImageKHR(driver->GetDevice(), SwapChain::GetVkSwapChain(), UINT64_MAX,
                                              GetCurrentAvailableSemaphore(), VK_NULL_HANDLE, &image_index);
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      SwapChain::ReInit(driver);
      return;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      LOG(FATAL) << "failed to acquire swap chain image:" << string_VkResult(result);
    }
  }
  vkResetFences(driver->GetDevice(), 1, &GetCurrentFence());

  CommandPool::ResetCommandBuffer(GetCurrentFrame());
  CommandPool::RecordCommandBuffer(GetCurrentFrame(), image_index);

  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore wait_semaphores[] = {GetCurrentAvailableSemaphore()};
  VkPipelineStageFlags wait_stages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = wait_semaphores;
  submit_info.pWaitDstStageMask = wait_stages;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &CommandPool::GetCommandBuffer(GetCurrentFrame());

  VkSemaphore signal_semaphores[] = {GetCurrentFinishedSemaphore()};
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = signal_semaphores;

  const auto& graphics_queue = driver->GetGraphicsQueue();
  CHECK_VK(FATAL, vkQueueSubmit(graphics_queue, 1, &submit_info, GetCurrentFence()), "failed to submit to vk graphics queue");

  VkPresentInfoKHR present_info{};
  present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  present_info.waitSemaphoreCount = 1;
  present_info.pWaitSemaphores = signal_semaphores;

  VkSwapchainKHR swapchains[] = {SwapChain::GetVkSwapChain()};
  present_info.pSwapchains = swapchains;
  present_info.swapchainCount = 1;
  present_info.pImageIndices = &image_index;

  const auto& present_queue = driver->GetPresentQueue();
  {
    const auto result = vkQueuePresentKHR(present_queue, &present_info);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized_) {
      resized_ = false;
      SwapChain::ReInit(driver);
    }
  }

  current_frame_ = ((GetCurrentFrame() + 1) % MAX_NUMBER_OF_FRAMES_IN_FLIGHT);
}
}  // namespace prt