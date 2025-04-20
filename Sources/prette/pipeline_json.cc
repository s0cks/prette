#include "prette/pipeline_json.h"

namespace prt::json {
void PipelineHandler::OnParseRasterizerCullMode(const std::string& value) {
  builder()->SetRasterizerCullMode(value);
}

void PipelineHandler::OnParseRasterizerFrontFace(const std::string& value) {
  builder()->SetRasterizerFrontFace(value);
}

void PipelineHandler::OnParseVertexShader(const std::string& value) {
  const auto shader = GetVertexShader(value);
  builder()->AttachVertexShader(shader);
}

void PipelineHandler::OnParseFragmentShader(const std::string& value) {
  const auto shader = GetFragmentShader(value);
  builder()->AttachFragmentShader(shader);
}

void PipelineHandler::OnParseDynamicState(const std::string& value) {
  if (value == "viewport") {
    builder()->dynamic_states_.push_back(VK_DYNAMIC_STATE_VIEWPORT);
  } else if (value == "scissor") {
    builder()->dynamic_states_.push_back(VK_DYNAMIC_STATE_SCISSOR);
  }
}
}  // namespace prt::json