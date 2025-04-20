#include "prette/skybox.h"

#include <vulkan/vulkan_core.h>

#include "prette/camera.h"
#include "prette/gfx.h"
#include "prette/texture.h"

namespace prt {
static std::array<float, 24> kSkyboxVertices = {
    -1.0f, 1.0f,  1.0f,   // vertex 0
    -1.0f, -1.0f, 1.0f,   // vertex 1
    1.0f,  -1.0f, 1.0f,   // vertex 2
    1.0f,  1.0f,  1.0f,   // vertex 3
    -1.0f, -1.0f, -1.0f,  // vertex 4
    -1.0f, 1.0f,  -1.0f,  // vertex 5
    1.0f,  -1.0f, -1.0f,  // vertex 6
    1.0f,  1.0f,  -1.0f   // vertex 7
};

static std::array<uint16_t, 36> kSkyboxIndices = {
    3, 2, 0, 0, 2, 1,  // face 1
    6, 2, 3, 7, 6, 3,  // face 2
    6, 7, 4, 7, 5, 4,  // face 3
    4, 0, 1, 5, 0, 4,  // face 4
    7, 3, 0, 5, 7, 0,  // face 5
    2, 6, 4, 1, 2, 4   // face 6
};

static Texture* texture_ = nullptr;
static vk::Buffer* data_buffer_ = nullptr;
static SkyboxMesh* mesh_ = nullptr;

static inline void InitVertexBuffer(const std::array<float, 24>& vertices, vk::Buffer** buffer, uint64_t* num_vertices) {
  const auto buffer_size = (sizeof(float) * vertices.size());
  const auto new_buffer = vk::Buffer::New(buffer_size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  ASSERT(new_buffer);
  new_buffer->CopyFromBytes(&vertices.at(0), buffer_size, true);
  (*buffer) = new_buffer;
  (*num_vertices) = vertices.size();
}

static inline void InitIndexBuffer(const std::array<uint16_t, 36>& indices, vk::Buffer** buffer, uint64_t* num_indices) {
  const auto buffer_size = (sizeof(uint16_t) * indices.size());
  const auto new_buffer = vk::Buffer::New(buffer_size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
  ASSERT(new_buffer);
  new_buffer->CopyFromBytes(&indices.at(0), buffer_size, true);
  (*buffer) = new_buffer;
  (*num_indices) = indices.size();
}

SkyboxMesh::SkyboxMesh() :
  vertices_(nullptr),
  num_vertices_(0),
  indices_(nullptr),
  num_indices_(0) {
  InitVertexBuffer(kSkyboxVertices, &vertices_, &num_vertices_);
  InitIndexBuffer(kSkyboxIndices, &indices_, &num_indices_);
}

SkyboxMesh::~SkyboxMesh() {
  delete vertices_;
  num_vertices_ = 0;
  delete indices_;
  num_indices_ = 0;
}

void SkyboxMesh::Destroy() {
  ASSERT(vertices_);
  vertices_->Destroy();
  ASSERT(indices_);
  indices_->Destroy();
}

void SkyboxMesh::Draw(VkCommandBuffer& buffer) {
  ASSERT(vertices_ && num_vertices_ > 0);
  ASSERT(indices_ && num_indices_ > 0);
  std::array<VkBuffer, 1> vertex_buffers = {vertices_->GetBuffer()};
  std::array<VkDeviceSize, 1> offsets = {0};
  vkCmdBindVertexBuffers(buffer, 0, 1, vertex_buffers.data(), offsets.data());
  vkCmdBindIndexBuffer(buffer, indices_->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);
  vkCmdDrawIndexed(buffer, num_indices_, 1, 0, 0, 0);
}

SkyboxPipeline::SkyboxPipeline() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  texture_ = new Texture(Texture::kCubeMap, "");
  ASSERT(texture_);
  std::vector<VkDescriptorSetLayout> dset_layouts = {
      dset_layout_,
  };
  InitLayout(driver, layout_, dset_layouts);
}

void SkyboxPipeline::InitDescriptorSet(const Driver* driver, VkDescriptorSetLayout& dset_layout, VkDescriptorSet& dset) {
  ASSERT(driver);
  VkDescriptorSetLayoutBinding ubo_binding{};
  ubo_binding.binding = 0;
  ubo_binding.descriptorCount = 1;
  ubo_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_binding.pImmutableSamplers = nullptr;
  ubo_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding sampler_binding{};
  sampler_binding.binding = 1;
  sampler_binding.descriptorCount = 1;
  sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_binding.pImmutableSamplers = nullptr;
  sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = {
      ubo_binding,
      sampler_binding,
  };
  VkDescriptorSetLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  create_info.bindingCount = bindings.size();
  create_info.pBindings = bindings.data();
  CHECK_VK(FATAL, vkCreateDescriptorSetLayout(driver->GetDevice(), &create_info, driver->GetAllocator(), &dset_layout),
           "failed to create vk descriptor set layout");

  VkDescriptorSetAllocateInfo alloc_info{};
  alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  alloc_info.pSetLayouts = &dset_layout;
  alloc_info.descriptorPool = driver->GetDescriptorPool();
  alloc_info.descriptorSetCount = 1;
  CHECK_VK(FATAL, vkAllocateDescriptorSets(driver->GetDevice(), &alloc_info, &dset), "failed to allocate vk descriptor set");

  VkDescriptorBufferInfo buffer_info{};
  buffer_info.buffer = data_buffer_->GetBuffer();
  buffer_info.offset = 0;
  buffer_info.range = sizeof(SkyboxData);

  std::array<VkWriteDescriptorSet, 2> writes{};
  writes.at(0).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes.at(0).dstSet = dset;
  writes.at(0).dstBinding = 0;
  writes.at(0).dstArrayElement = 0;
  writes.at(0).descriptorCount = 1;
  writes.at(0).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  writes.at(0).pBufferInfo = &buffer_info;

  VkDescriptorImageInfo image_info{};
  image_info.sampler = texture_->GetSampler();
  image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  image_info.imageView = texture_->GetImageView();

  writes.at(1).sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  writes.at(1).dstSet = dset;
  writes.at(1).dstBinding = 1;
  writes.at(1).dstArrayElement = 0;
  writes.at(1).descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  writes.at(1).descriptorCount = 1;
  writes.at(1).pImageInfo = &image_info;
  vkUpdateDescriptorSets(driver->GetDevice(), writes.size(), writes.data(), 0, nullptr);
}

void SkyboxPipeline::InitLayout(const Driver* driver, VkPipelineLayout& layout,
                                const std::vector<VkDescriptorSetLayout>& dset_layouts) {
  ASSERT(driver);
  VkPipelineLayoutCreateInfo create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  create_info.setLayoutCount = dset_layouts.size();
  create_info.pSetLayouts = dset_layouts.data();
  CHECK_VK(FATAL, vkCreatePipelineLayout(driver->GetDevice(), &create_info, driver->GetAllocator(), &layout),
           "failed to create vk pipeline layout");
}

SkyboxPipeline::~SkyboxPipeline() {
  const auto driver = Driver::Get();
  ASSERT(driver);
  delete texture_;
  vkDestroyPipeline(driver->GetDevice(), pipeline_, driver->GetAllocator());
  vkDestroyPipelineLayout(driver->GetDevice(), layout_, driver->GetAllocator());
  vkDestroyPipelineCache(driver->GetDevice(), cache_, driver->GetAllocator());
}

void SkyboxPipeline::Draw(const VkCommandBuffer& buffer) {
  static constexpr const std::array<VkDeviceSize, 1> kDefaultOffsets = {0};
  vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout_, 0, 1, &dset_, 0, nullptr);
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_);
  vkCmdBindVertexBuffers(buffer, 0, 1, &mesh_->GetVertexBuffer()->GetBuffer(), kDefaultOffsets.data());
  vkCmdBindIndexBuffer(buffer, mesh_->GetIndexBuffer()->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(buffer, mesh_->GetNumberOfIndices(), 1, 0, 0, 0);
}

void Skybox::Init() {
  data_buffer_ =
      vk::Buffer::NewUniformBuffer<SkyboxData>(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
  ASSERT(data_buffer_);
}

void Skybox::Update() {
  ASSERT(data_buffer_);
  const auto camera = ((PerspectiveCamera*)Camera::Get());
  ASSERT(camera);

  SkyboxData data{};
  data.view = camera->GetProjection() * glm::mat4(glm::mat3(camera->GetView()));
  data_buffer_->CopyFromBytes(&data, sizeof(SkyboxData));
}
}  // namespace prt