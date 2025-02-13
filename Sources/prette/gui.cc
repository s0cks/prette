#include "prette/gui.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <stb_image.h>
#include <vulkan/vulkan_core.h>

#include "prette/engine.h"
#include "prette/flags.h"
#include "prette/gfx.h"
#include "prette/gfx_driver.h"
#include "prette/gui_renderer.h"
#include "prette/mouse.h"
#include "prette/renderer.h"
#include "prette/shader.h"
#include "prette/window.h"

namespace prt::gui {
struct VkFont {
  VkImage image{};
  VkDeviceMemory memory{};
  VkImageView view{};
};

// NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers)
static inline void InitStyle() {
  ImGui::StyleColorsDark();
}
// NOLINTEND(cppcoreguidelines-avoid-magic-numbers)

enum class TextureType { DIFFUSE_MAP = 0, SPECULAR_MAP = 1, NORMAL_MAP = 2, CUBE_MAP = 3 };

using TextureBytes = std::unique_ptr<uint8_t[], std::function<void(uint8_t*)>>;

struct TextureData {
  TextureBytes data;
  int width;
  int height;
  int num_channels;
};

static inline auto ReadTexture(const std::string& filename, const bool flip = false) -> TextureData {
  const auto image_path = fs::path(FLAGS_resources) / "textures" / filename;
  const int force_channels = STBI_rgb_alpha;

  int width = 0, height = 0, num_channels = 0;
  TextureBytes bytes = {stbi_load(image_path.c_str(), &width, &height, &num_channels, force_channels),
                        (void (*)(uint8_t*))stbi_image_free};
  return {.data = std::move(bytes), .width = width, .height = height, .num_channels = num_channels};
}

class Texture {
 public:
  Texture(TextureType type, std::string_view textureName);

  Texture() = default;

  void Destroy() {
    const auto driver = Driver::Get();
    ASSERT(driver);
    vkDestroySampler(driver->GetDevice(), m_textureSampler, nullptr);
    vkDestroyImageView(driver->GetDevice(), m_textureImageView, nullptr);
    vkDestroyImage(driver->GetDevice(), m_textureImage, nullptr);
    vkFreeMemory(driver->GetDevice(), m_textureImageMemory, nullptr);
  }

  void CreateTextureImage(TextureType type, std::string textureName) {
    m_type = type;
    auto textureData = ReadTexture(textureName);
    m_width = textureData.width;
    m_height = textureData.height;

    m_format = type == TextureType::DIFFUSE_MAP ? VK_FORMAT_R8G8B8A8_SRGB : VK_FORMAT_R8G8B8A8_UNORM;
    m_mips = static_cast<uint32_t>(std::floor(std::log2(std::max(m_width, m_height)))) + 1;

    std::tie(m_textureImage, m_textureImageMemory) =
        CreateImage(m_width, m_height, m_mips, VK_SAMPLE_COUNT_1_BIT, m_format, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, type == TextureType::CUBE_MAP);

    m_textureImageView =
        CreateImageView(m_textureImage, m_format, VK_IMAGE_ASPECT_COLOR_BIT, m_mips, type == TextureType::CUBE_MAP);

    CreateTextureSampler();

    TransitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mips);

    CopyBufferToImage(textureData.data.get());

    // transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps
    GenerateMipmaps(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, static_cast<int32_t>(m_width), static_cast<int32_t>(m_height),
                    m_mips);
  }

  static auto CreateImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format,
                          VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, bool cubemap = false)
      -> std::pair<VkImage, VkDeviceMemory> {
    VkImage image{};
    VkDeviceMemory imageMemory{};

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (cubemap) {
      // Cube faces count as array layers in Vulkan
      imageInfo.arrayLayers = 6;
      // This flag is required for cube map images
      imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    const auto driver = Driver::Get();
    ASSERT(driver);
    CHECK_VK(FATAL, vkCreateImage(driver->GetDevice(), &imageInfo, nullptr, &image), "failed to create image!");

    vk::Buffer::AllocateImageMemory(driver, image, imageMemory, properties);
    vkBindImageMemory(driver->GetDevice(), image, imageMemory, 0);
    return {image, imageMemory};
  }

  static void GenerateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels) {
    const auto driver = Driver::Get();
    ASSERT(driver);

    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(driver->GetPhysicalDevice(), imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
      LOG(FATAL) << "Texture image format does not support linear blitting!";
    }

    SingleUseCommandBuffer buffer(Renderer::GetCommandPool());
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
      barrier.subresourceRange.baseMipLevel = i - 1;
      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

      vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1,
                           &barrier);

      VkImageBlit blit{};
      blit.srcOffsets[0] = {0, 0, 0};
      blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
      blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.srcSubresource.mipLevel = i - 1;
      blit.srcSubresource.baseArrayLayer = 0;
      blit.srcSubresource.layerCount = 1;
      blit.dstOffsets[0] = {0, 0, 0};
      blit.dstOffsets[1] = {mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1};
      blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      blit.dstSubresource.mipLevel = i;
      blit.dstSubresource.baseArrayLayer = 0;
      blit.dstSubresource.layerCount = 1;

      vkCmdBlitImage(buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit,
                     VK_FILTER_LINEAR);

      barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
      barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0,
                           nullptr, 1, &barrier);

      if (mipWidth > 1) {
        mipWidth /= 2;
      }

      if (mipHeight > 1) {
        mipHeight /= 2;
      }
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr,
                         1, &barrier);
  }

  static auto CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels,
                              bool cubemap = false) -> VkImageView {
    const auto driver = Driver::Get();
    ASSERT(driver);
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = not cubemap ? VK_IMAGE_VIEW_TYPE_2D : VK_IMAGE_VIEW_TYPE_CUBE;
    viewInfo.format = format;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = not cubemap ? 1 : 6;

    VkImageView imageView{};
    CHECK_VK(FATAL, vkCreateImageView(driver->GetDevice(), &viewInfo, driver->GetAllocator(), &imageView),
             "Failed to create texture image view!");

    return imageView;
  }

  static inline auto CreateSampler(uint32_t mipLevels = 1) -> VkSampler {
    const auto driver = Driver::Get();
    ASSERT(driver);
    VkSampler sampler{};

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(driver->GetPhysicalDevice(), &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0.0f;
    CHECK_VK(FATAL, vkCreateSampler(driver->GetDevice(), &samplerInfo, nullptr, &sampler), "Failed to create texture sampler!");
    return sampler;
  }

  static void TransitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels,
                                    bool cubemap = false) {
    SingleUseCommandBuffer buffer(Renderer::GetCommandPool());

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = not cubemap ? 1 : 6;

    VkPipelineStageFlags sourceStage = {};
    VkPipelineStageFlags destinationStage = {};

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
      barrier.srcAccessMask = 0;
      barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

      sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
      destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
      barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
      barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

      sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
      destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
      LOG(FATAL) << "invalid layout transition";
    }

    vkCmdPipelineBarrier(buffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
  }

  static void CopyBufferToCubemapImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data);

  auto GetImageViewAndSampler() const -> std::pair<VkImageView, VkSampler> {
    return {m_textureImageView, m_textureSampler};
  }

  void CreateTextureSampler() {
    m_textureSampler = CreateSampler(m_mips);
  }

  auto GetType() const -> TextureType {
    return m_type;
  }

  void CopyBufferToImage(uint8_t* data) {
    return CopyBufferToImage(m_textureImage, m_width, m_height, data);
  }

  static void CopyBufferToImage(VkImage image, uint32_t texWidth, uint32_t texHeight, uint8_t* data) {
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = {.x = 0, .y = 0, .z = 0};
    region.imageExtent = {.width = texWidth, .height = texHeight, .depth = 1};

    const auto size = static_cast<size_t>(texWidth) * static_cast<size_t>(texHeight) * size_t{4};
    vk::Buffer::CopyDataToImageWithStaging(image, data, size, {region});
  }

 private:
  void TransitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels) {
    TransitionImageLayout(m_textureImage, oldLayout, newLayout, mipLevels);
  }

 private:
  TextureType m_type = {};
  VkImage m_textureImage = {};
  VkDeviceMemory m_textureImageMemory = {};
  VkImageView m_textureImageView = {};
  VkSampler m_textureSampler = {};
  VkFormat m_format = {};
  // int32_t m_channels = {};
  uint32_t m_mips = {};
  uint32_t m_width = {};
  uint32_t m_height = {};
  std::string m_name = "default_texture_name";
};

static size_t kRenderTarget = 0;

auto Update(const glm::u32vec2& size) -> bool {
  auto& io = ImGui::GetIO();
  io.DisplaySize = ImVec2(static_cast<float>(size.x), static_cast<float>(size.y));
  io.DisplayFramebufferScale = ImVec2(2.0f, 2.0f);

  const auto mouse = Mouse::Get();
  ASSERT(mouse);
  const auto pos = mouse->GetPos();
  io.MousePos = ImVec2(static_cast<float>(pos.x), static_cast<float>(pos.y));
  io.MouseDown[0] = mouse->IsPressed(GLFW_MOUSE_BUTTON_1);
  io.MouseDown[1] = mouse->IsPressed(GLFW_MOUSE_BUTTON_2);

  ImGui_ImplVulkan_NewFrame();
  ImGui::NewFrame();
  ImGui::Begin("Viewport");
  ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
  ImGui::Image(GuiRenderer::GetSceneDescriptor(Renderer::GetCurrentFrame()), ImVec2{viewportPanelSize.x, viewportPanelSize.y});

  // const auto target_items = std::array<const char*, 2>{
  //     "Full Scene",
  //     "Test",
  // };
  // if (ImGui::BeginCombo("Target", target_items.at(kRenderTarget), ImGuiComboFlags_HeightSmall)) {
  //   for (auto idx = 0; idx < target_items.size(); idx++) {
  //     const auto is_selected = (kRenderTarget == idx);
  //     if (ImGui::Selectable(target_items.at(idx), is_selected)) {
  //       kRenderTarget = idx;
  //     }
  //     if (is_selected) {
  //       ImGui::SetItemDefaultFocus();
  //     }
  //   }
  //   ImGui::EndCombo();
  // }
  ImGui::End();
  ImGui::Render();
  return true;
}

static void Destroy(const Driver* driver) {
  ASSERT(driver);
}

void Init() {
  OnDriverInitEvent().subscribe([](DriverInitEvent* event) {
    ASSERT(event);
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    const auto window = GetAppWindow();
    ASSERT(window);
    ImGui_ImplGlfw_InitForVulkan(window->GetHandle(), true);
  });
  engine::OnTickEvent().subscribe([](engine::TickEvent* event) {
    ASSERT(event);
    const auto window = GetAppWindow();
    ASSERT(window);
    const auto size = window->GetSize();
    if (!Update(size.data())) {
      // update more?
    }
  });
  engine::OnTerminatingEvent().subscribe([](engine::TerminatingEvent* event) {
    ASSERT(event);
    const auto driver = Driver::Get();
    ASSERT(driver);
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  });
}
}  // namespace prt::gui