#ifndef PRT_PIPELINE_JSON_H
#define PRT_PIPELINE_JSON_H

#include <fmt/format.h>
#include <glog/logging.h>
#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>

#include "prette/common.h"
#include "prette/pipeline.h"
#include "prette/pipeline_builder.h"
#include "vulkan/vulkan_core.h"

namespace prt {
class GraphicsPipeline;

namespace json {
using namespace rapidjson;

#define FOR_EACH_PIPELINE_HANDLER_STATE(V) \
  V(OpenDoc)                               \
  V(ParsingType)                           \
  V(ExpectMeta)                            \
  V(ParsingMeta)                           \
  V(ParsingName)                           \
  V(ExpectData)                            \
  V(ParsingData)                           \
  V(ParsingVertexShader)                   \
  V(ParsingFragmentShader)                 \
  V(ParsingCullMode)                       \
  V(ParsingFrontFace)                      \
  V(ExpectDynamicStates)                   \
  V(ParsingDynamicStates)                  \
  V(ClosedDoc)                             \
  V(Error)

class PipelineHandler : public BaseReaderHandler<UTF8<>, PipelineHandler> {
  friend class prt::GraphicsPipeline;  // TODO: remove

  enum State {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_PIPELINE_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

  friend auto operator<<(std::ostream& stream, const State& rhs) -> std::ostream& {
    switch (rhs) {
#define DEFINE_TO_STRING(Name) \
  case k##Name:                \
    return stream << #Name;

      FOR_EACH_PIPELINE_HANDLER_STATE(DEFINE_TO_STRING)
#undef DEFINE_TO_STRING
      default:
        return stream << "invalid state.";
    }
  }

  struct KeyState {
    const char* name;
    State state;

    auto operator==(const std::string& rhs) const -> bool {
      return name == rhs;
    }

    operator State() const {
      return state;
    }
  };

 private:
  State state_ = kClosedDoc;
  std::string error_{};
  BaseRenderPipelineBuilder* builder_;

  inline auto builder() const -> BaseRenderPipelineBuilder* {
    return builder_;
  }

  inline auto Continue() -> bool {
    return true;
  }

  auto GetState() const -> State {
    return state_;
  }

  inline void SetState(const State rhs) {
    state_ = rhs;
  }

  inline void SetError(const std::string& rhs) {
    ASSERT(!rhs.empty());
    error_ = rhs;
  }

  inline auto Error(const std::string& rhs) -> bool {
    SetState(kError);
    SetError(rhs);
    return false;
  }

  inline auto InvalidState() -> bool {
    return Error("Invalid State");
  }

  inline auto TransitionTo(const State rhs) -> bool {
    SetState(rhs);
    return Continue();
  }

  template <typename Container>
  inline auto CheckKeyTable(const std::string& key, const Container& key_table) -> bool {
    for (const auto& entry : key_table) {
      if (key == entry)
        return TransitionTo(entry);
    }
    return Error(fmt::format("invalid key `{}`", key));
  }

  void OnParseRasterizerCullMode(const std::string& value);
  void OnParseRasterizerFrontFace(const std::string& value);
  void OnParseVertexShader(const std::string& value);
  void OnParseFragmentShader(const std::string& value);
  void OnParseDynamicState(const std::string& value);

 public:
  explicit PipelineHandler(BaseRenderPipelineBuilder* builder) :
    builder_(builder) {}
  ~PipelineHandler() = default;

  auto HasError() const -> bool {
    return !error_.empty();
  }

  auto GetError() const -> const std::string& {
    return error_;
  }

  auto Null() -> bool {
    return InvalidState();
  }

  auto Bool(bool b) -> bool {
    return InvalidState();
  }

  auto Int(int i) -> bool {
    return InvalidState();
  }

  auto Uint(unsigned u) -> bool {
    return InvalidState();
  }

  auto Int64(int64_t i) -> bool {
    return InvalidState();
  }

  auto Uint64(uint64_t u) -> bool {
    return InvalidState();
  }

  auto Double(double d) -> bool {
    return InvalidState();
  }

  auto String(const char* str, SizeType length, bool copy) -> bool {
    const auto value = std::string(str, length);
    switch (GetState()) {
      case kParsingType: {
        if (value != "Pipeline")
          return Error(fmt::format("unexpected value for 'type' field: `{}`", value));
        return TransitionTo(kOpenDoc);
      }
      case kParsingName:
        return TransitionTo(kParsingMeta);
      case kParsingCullMode:
        OnParseRasterizerCullMode(value);
        return TransitionTo(kParsingData);
      case kParsingFrontFace:
        OnParseRasterizerFrontFace(value);
        return TransitionTo(kParsingData);
      case kParsingVertexShader:
        OnParseVertexShader(value);
        return TransitionTo(kParsingData);
      case kParsingFragmentShader:
        OnParseFragmentShader(value);
        return TransitionTo(kParsingData);
      case kParsingDynamicStates:
        OnParseDynamicState(value);
        return Continue();
      default:
        break;
    }
    return InvalidState();
  }

  auto StartObject() -> bool {
    switch (GetState()) {
      case kClosedDoc:
        return TransitionTo(kOpenDoc);
      case kExpectMeta:
        return TransitionTo(kParsingMeta);
      case kExpectData:
        return TransitionTo(kParsingData);
      default:
        break;
    }
    return InvalidState();
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool {
    const auto key = std::string(str, length);
    switch (GetState()) {
      case kOpenDoc: {
        static const std::array<KeyState, 3> kPipelineKeys = {
            KeyState{.name = "type", .state = kParsingType},
            KeyState{.name = "meta", .state = kExpectMeta},
            KeyState{.name = "data", .state = kExpectData},
        };
        return CheckKeyTable(key, kPipelineKeys);
      }
      case kParsingMeta: {
        static const std::array<KeyState, 1> kMetaKeys = {
            KeyState{.name = "name", .state = kParsingName},
        };
        return CheckKeyTable(key, kMetaKeys);
      }
      case kParsingData: {
        static const std::array<KeyState, 5> kDataKeys = {
            KeyState{.name = "cullMode", .state = kParsingCullMode},
            KeyState{.name = "frontFace", .state = kParsingFrontFace},
            KeyState{.name = "vertexShader", .state = kParsingVertexShader},
            KeyState{.name = "fragmentShader", .state = kParsingFragmentShader},
            KeyState{.name = "dynamicStates", .state = kExpectDynamicStates},
        };
        return CheckKeyTable(key, kDataKeys);
      }
      default:
        break;
    }
    return InvalidState();
  }

  auto EndObject(SizeType memberCount) -> bool {
    switch (GetState()) {
      case kOpenDoc:
        return TransitionTo(kClosedDoc);
      case kParsingMeta:
      case kParsingData:
        return TransitionTo(kOpenDoc);
      case kParsingCullMode:
      case kParsingFrontFace:
      case kParsingVertexShader:
      case kParsingFragmentShader:
        return TransitionTo(kParsingData);
      default:
        break;
    }
    return InvalidState();
  }

  auto StartArray() -> bool {
    switch (GetState()) {
      case kExpectDynamicStates:
        return TransitionTo(kParsingDynamicStates);
      default:
        break;
    }
    return InvalidState();
  }

  auto EndArray(SizeType elementCount) -> bool {
    switch (GetState()) {
      case kParsingDynamicStates:
        return TransitionTo(kParsingData);
      default:
        break;
    }
    return InvalidState();
  }
};

inline auto ReadJsonFromFile(fs::path path, std::string& result) -> bool {
  if (!fs::exists(path)) {
    LOG(ERROR) << "failed to find ShaderCode at: " << path;
    return false;
  }
  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    LOG(ERROR) << "failed to open ShaderCode at: " << path;
    return false;
  }
  auto filesize = static_cast<std::streamsize>(file.tellg());
  result.resize(filesize + 1);
  file.seekg(0);
  file.read((char*)result.data(), filesize);
  file.close();
  result[filesize] = '\0';
  return true;
}

template <typename H>
void ParseJsonDocumentFrom(fs::path path, H& handler) {
  std::string buffer{};
  LOG_IF(FATAL, !ReadJsonFromFile(path, buffer)) << "failed to read json from file: " << path;
  json::Reader reader{};
  json::StringStream ss(buffer.data());
  if (!reader.Parse(ss, handler)) {
    json::ParseErrorCode e = reader.GetParseErrorCode();
    size_t o = reader.GetErrorOffset();
    std::cerr << "Error: ";
    if (handler.HasError()) {
      std::cerr << handler.GetError();
    } else {
      std::cerr << json::GetParseError_En(e);
    }
    std::cerr << std::endl;
    std::cerr << " at offset " << o << " near '" << std::string(buffer.data()).substr(o, 10) << "...'" << std::endl;
  }
}
}  // namespace json
}  // namespace prt

#endif  // PRT_PIPELINE_JSON_H
