#include "prette/gui_renderer.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <vulkan/vulkan_core.h>

#include "prette/common.h"
#include "prette/gfx.h"
#include "prette/gfx_vk.h"
#include "prette/renderer.h"
#include "prette/scene_renderer.h"
#include "prette/shader.h"
#include "prette/swapchain.h"

namespace prt {
static VkRenderPass pass_{};
static std::vector<VkFramebuffer> framebuffers_{};
static std::array<VkCommandBuffer, MAX_NUMBER_OF_FRAMES_IN_FLIGHT> command_buffers_{};
static std::vector<VkDescriptorSet> descriptors_{};

static VkSampler sampler_{};
static GuiRendererEventSubject events_{};

auto OnGuiRendererEvent() -> GuiRendererEventObservable {
  return events_.get_observable();
}

static inline void PublishEvent(GuiRendererEvent* event) {
  ASSERT(event);
  const auto& subscriber = events_.get_subscriber();
  return subscriber.on_next(event);
}

template <class E, typename... Args>
static inline void Publish(Args... args) {
  E event(args...);
  return PublishEvent(&event);
}

void GuiRenderer::InitSampler(const Driver* driver) {
  ASSERT(driver);
  VkSamplerCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  create_info.magFilter = VK_FILTER_LINEAR;
  create_info.minFilter = VK_FILTER_LINEAR;
  create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  create_info.anisotropyEnable = VK_FALSE;
  create_info.maxAnisotropy = 1.0f;
  create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
  create_info.unnormalizedCoordinates = VK_FALSE;
  create_info.compareEnable = VK_FALSE;
  create_info.compareOp = VK_COMPARE_OP_ALWAYS;
  create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  create_info.mipLodBias = 0.0f;
  create_info.minLod = 0.0f;
  create_info.maxLod = 0.0f;
  CHECK_VK(FATAL, vkCreateSampler(driver->GetDevice(), &create_info, nullptr, &sampler_), "failed to create vk sampler");
}

void GuiRenderer::InitCommandBuffers(const Driver* driver) {
  VkCommandBufferAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  alloc_info.commandPool = driver->GetCommandPool();
  alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  alloc_info.commandBufferCount = command_buffers_.size();
  CHECK_VK(FATAL, vkAllocateCommandBuffers(driver->GetDevice(), &alloc_info, command_buffers_.data()),
           "failed to allocate vk command buffers");
}

void GuiRenderer::InitRenderPass(const Driver* driver) {
  VkAttachmentDescription color_attachment{};
  color_attachment.format = SwapChain::GetFormat();
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  VkAttachmentReference color_attachment_ref{};
  color_attachment_ref.attachment = 0;
  color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_ref;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkRenderPassCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  create_info.attachmentCount = 1;
  create_info.pAttachments = &color_attachment;
  create_info.subpassCount = 1;
  create_info.pSubpasses = &subpass;
  create_info.dependencyCount = 1;
  create_info.pDependencies = &dependency;

  CHECK_VK(FATAL, vkCreateRenderPass(driver->GetDevice(), &create_info, nullptr, &pass_), "failed to create vk render pass");
}

void GuiRenderer::InitFramebuffers(const Driver* driver) {
  ASSERT(driver);
  vk::InitFramebuffers(driver, pass_, SwapChain::GetImageViews(), SwapChain::GetExtent(), framebuffers_);
}

void GuiRenderer::Destroy(const Driver* driver, const bool is_reinit) {
  ASSERT(driver);
  if (!is_reinit) {
    DLOG(INFO) << "destroying....";
    vkDestroyRenderPass(driver->GetDevice(), pass_, driver->GetAllocator());
  }
  for (const auto& framebuffer : framebuffers_) {
    vkDestroyFramebuffer(driver->GetDevice(), framebuffer, driver->GetAllocator());
  }
  if (!is_reinit) {
    vkDestroySampler(driver->GetDevice(), sampler_, driver->GetAllocator());
    vkFreeCommandBuffers(driver->GetDevice(), driver->GetCommandPool(), command_buffers_.size(), command_buffers_.data());
  }
}

auto GuiRenderer::GetPass() -> VkRenderPass const& {
  return pass_;
}

auto GuiRenderer::GetSceneDescriptor(const uint32_t frame) -> VkDescriptorSet const& {
  return descriptors_[frame];
}

void GuiRenderer::Init() {
  OnSwapChainInitEvent().subscribe([](SwapChainInitEvent* event) {
    try {
      ASSERT(event);
      const auto driver = Driver::Get();
      ASSERT(driver);
      if (!event->IsReinit()) {
        DLOG(INFO) << "iniializing GuiRenderer....";
        InitRenderPass(driver);
      }
      InitFramebuffers(driver);
      if (!event->IsReinit()) {
        InitCommandBuffers(driver);
        ImGui_ImplVulkan_InitInfo info{};
        info.Instance = driver->GetInstance();
        info.PhysicalDevice = driver->GetPhysicalDevice();
        info.Allocator = driver->GetAllocator();
        info.Device = driver->GetDevice();
        info.Queue = driver->GetGraphicsQueue();
        info.DescriptorPool = driver->GetDescriptorPool();
        info.ImageCount = 2;
        info.MinImageCount = 2;
        info.RenderPass = GetPass();
        info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&info);

        ImGui_ImplVulkan_CreateFontsTexture();
        ImGui_ImplVulkan_DestroyFontsTexture();

        InitSampler(driver);
        descriptors_.resize(SwapChain::GetNumberOfImages());
        for (auto idx = 0; idx < SwapChain::GetNumberOfImages(); idx++) {
          descriptors_[idx] =
              ImGui_ImplVulkan_AddTexture(sampler_, SceneRenderer::GetImageView(idx), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
      }
    } catch (const std::exception& exc) {
      LOG(FATAL) << "exception: " << exc.what();
    }
  });
  OnSwapChainDestroyedEvent().subscribe([](SwapChainDestroyedEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    Destroy(driver, event->IsReinit());
  });
}

void GuiRenderer::Draw(const SwapChainFrame& frame, std::vector<VkCommandBuffer>& cmd_buffers) {
  // clang-format off
  static const std::vector<VkClearValue> kClearValues = {
    VkClearValue{.color = {0.0f, 0.0f, 0.0f, 1.0f }}
  };
  // clang-format on
  CommandBufferScope buffer(command_buffers_.at(frame));
  {
    RenderPassScope render_pass(buffer, pass_, framebuffers_[frame.image], kClearValues);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), buffer);
  }
  cmd_buffers.push_back(buffer);
}
}  // namespace prt