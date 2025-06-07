#include "prette/pipeline/pipeline_layout_json.h"

#include <string>

#include "prette/assertions.h"
#include "prette/camera_manager.h"
#include "prette/chunk/chunk_metadata.h"
#include "prette/common.h"
#include "prette/descriptor_set.h"
#include "prette/json.h"
#include "prette/material/material_system.h"
#include "prette/pipeline/pipeline_layout_builder.h"
#include "prette/pipeline/pipeline_layout_manager.h"
#include "prette/render_pass/scene_renderer.h"
#include "prette/vk.h"

namespace prt::json {
PipelineLayoutHandler::PipelineLayoutHandler(vk::PipelineLayoutBuilder* builder) :
  BaseStatefulReaderHandler<PipelineLayoutHandlerState, PipelineLayoutHandler>(PipelineLayoutHandlerState::kEmpty),
  builder_(builder) {}

auto PipelineLayoutHandler::Key(const char* str, SizeType length, bool copy) -> bool {
  ASSERT(IsOpenDoc());
  std::string key(str, length);
  if (EqualsIgnoreCase(key, "descriptor_sets"))
    return TransitionToParsingDescriptorSets();
  else if (EqualsIgnoreCase(key, "push_constants"))
    return TransitionToParsingPushConstants();
  return InvalidState();
}

auto PipelineLayoutHandler::OnParseDescriptorSetString(const std::string& value) -> bool {
  if (EqualsIgnoreCase(value, "material")) {
    builder_->WithDescriptorSetLayout(MaterialSystem::GetSystem()->GetMaterialDescriptorSetLayout());
    return Continue();
  } else if (EqualsIgnoreCase(value, "camera")) {
    builder_->WithDescriptorSetLayouts(GetCameraManager()->GetDescriptorSet());
    return Continue();
  } else if (EqualsIgnoreCase(value, "tile")) {
    builder_->WithDescriptorSetLayout(GetSceneRenderer()->GetTileDescriptorSetLayout());
    return Continue();
  }

  const auto dset = vk::FindDescriptorSet(value);
  LOG_IF(FATAL, !vk::IsInitialized(dset)) << "failed to find descriptor set: " << value;
  builder_->WithDescriptorSetLayout(&dset->GetLayout());
  return Continue();
}

auto PipelineLayoutHandler::OnParsePushConstantString(const std::string& value) -> bool {
  if (EqualsIgnoreCase(value, "chunk")) {
    builder_->AddPushConstantRange(0, sizeof(ChunkData), VK_SHADER_STAGE_VERTEX_BIT);
    return TransitionToOpenDoc();
  }
  return InvalidState();
}

auto PipelineLayoutHandler::String(const char* str, SizeType length, bool copy) -> bool {
  std::string value(str, length);
  if (IsEmpty()) {
    const auto layout = vk::GetPipelineLayoutManager()->FindOrLoadPipelineLayout(value);
    LOG_IF(FATAL, !vk::IsInitialized(layout)) << "failed to find valid pipeline layout: " << value;
    SetResult(layout);
    return TransitionToClosedDoc();
  } else if (IsParsingDescriptorSets()) {
    return OnParseDescriptorSetString(value);
  } else if (IsParsingPushConstants()) {
    return OnParsePushConstantString(value);
  }
  return InvalidState();
}

auto PipelineLayoutHandler::StartObject() -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kEmpty:
      return TransitionToOpenDoc();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::EndObject(SizeType memberCount) -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kOpenDoc:
      return TransitionToClosedDoc();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::StartArray() -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kParsingDescriptorSets:
      return Continue();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::EndArray(SizeType elementCount) -> bool {
  switch (GetState()) {
    case PipelineLayoutHandlerState::kParsingDescriptorSets:
      return TransitionToOpenDoc();
    default:
      break;
  }
  return InvalidState();
}

auto PipelineLayoutHandler::Null() -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Bool(bool b) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Int(int i) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Uint(unsigned u) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Int64(int64_t i) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Uint64(uint64_t u) -> bool {
  return InvalidState();
}

auto PipelineLayoutHandler::Double(double d) -> bool {
  return InvalidState();
}
}  // namespace prt::json