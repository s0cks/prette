#include "prette/ubo/ubo_scope.h"
#include "prette/ubo/ubo.h"

namespace prt::ubo {
  void UboUpdateScope::Update(const uint64_t offset, const uint8_t* bytes, const uint64_t num_bytes) {
    Ubo::UpdateUbo(offset, bytes, num_bytes);
  }
}