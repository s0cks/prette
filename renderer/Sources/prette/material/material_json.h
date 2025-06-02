#ifndef PRT_MATERIAL_JSON_H
#define PRT_MATERIAL_JSON_H

#include <cstdint>

#include "prette/assertions.h"
#include "prette/json.h"
#include "prette/material/material.h"
namespace prt::json {

#define FOR_EACH_MATERIAL_HANDLER_STATE(V) \
  V(Empty)                                 \
  V(OpenDoc)                               \
  V(ParsingAlbedo)                         \
  V(ParsingNormal)                         \
  V(ParsingRoughness)                      \
  V(ParsingAo)                             \
  V(ClosedDoc)

enum class MaterialReaderHandlerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_MATERIAL_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

class MaterialReaderHandler : public BaseStatefulReaderHandler<MaterialReaderHandlerState, MaterialReaderHandler> {
 private:
  MaterialBuilder* builder_;

  inline auto builder() const -> MaterialBuilder* {
    return builder_;
  }

#define DEFINE_TRANSITION_TO(Name)                            \
  inline auto TransitionTo##Name()->bool {                    \
    return TransitionTo(MaterialReaderHandlerState::k##Name); \
  }
  FOR_EACH_MATERIAL_HANDLER_STATE(DEFINE_TRANSITION_TO)
#undef DEFINE_TRANSITION_TO

 public:
  explicit MaterialReaderHandler(MaterialBuilder* builder) :
    BaseStatefulReaderHandler<MaterialReaderHandlerState, MaterialReaderHandler>(MaterialReaderHandlerState::kEmpty),
    builder_(builder) {
    ASSERT(builder_);
  }
  ~MaterialReaderHandler() override = default;

#define DEFINE_STATE_CHECK(Name)                              \
  inline auto Is##Name() const->bool {                        \
    return GetState() == MaterialReaderHandlerState::k##Name; \
  }
  FOR_EACH_MATERIAL_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECk

  auto Null() -> bool override;
  auto Bool(bool b) -> bool override;
  auto Int(int i) -> bool override;
  auto Uint(unsigned u) -> bool override;
  auto Int64(int64_t i) -> bool override;
  auto Uint64(uint64_t u) -> bool override;
  auto Double(double d) -> bool override;
  auto Key(const char* str, SizeType length, bool copy) -> bool override;
  auto String(const char* str, SizeType length, bool copy) -> bool override;
  auto StartObject() -> bool override;
  auto EndObject(SizeType memberCount) -> bool override;
  auto StartArray() -> bool override;
  auto EndArray(SizeType elementCount) -> bool override;
};

}  // namespace prt::json

#endif  // PRT_MATERIAL_JSON_H
