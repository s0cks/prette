#include "prette/vk_queue.h"

#include "prette/device.h"
#include "prette/swapchain.h"
#include "prette/vk.h"

namespace prt::vk {
QueueSubmit::QueueSubmit() :
  QueueSubmit(Driver::Get()->GetDevice()->GetGraphicsQueue()) {}

QueueSubmit::QueueSubmit(VkQueue queue) :
  queue_(queue) {
  info_ptr()->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
}

auto QueueSubmit::Submit(const VkFence& fence) -> vk::Result {
  info_ptr()->waitSemaphoreCount = wait_semaphores_.size();
  info_ptr()->pWaitSemaphores = wait_semaphores_.data();
  info_ptr()->pWaitDstStageMask = wait_stages_.data();
  info_ptr()->commandBufferCount = command_buffers_.size();
  info_ptr()->pCommandBuffers = command_buffers_.data();
  info_ptr()->signalSemaphoreCount = signal_semaphores_.size();
  info_ptr()->pSignalSemaphores = signal_semaphores_.data();
  const auto driver = Driver::Get();
  return vkQueueSubmit(queue_, 1, info_ptr(), fence);
}

QueuePresent::QueuePresent() :
  QueuePresent(Driver::Get()->GetDevice()->GetPresentQueue()) {}

QueuePresent::QueuePresent(VkQueue queue) :
  queue_(queue) {
  info_ptr()->sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
}

auto QueuePresent::WithSwap(Swapchain* rhs) -> QueuePresent& {
  return WithSwap(*rhs);
}

auto QueuePresent::Present() -> Result {
  info_ptr()->pImageIndices = images_.data();
  info_ptr()->pSwapchains = swapchains_.data();
  info_ptr()->swapchainCount = swapchains_.size();
  info_ptr()->pWaitSemaphores = wait_semaphores_.data();
  info_ptr()->waitSemaphoreCount = wait_semaphores_.size();
  return vkQueuePresentKHR(queue_, info_ptr());
}
}  // namespace prt::vk