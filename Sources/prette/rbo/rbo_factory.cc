#include "prette/rbo/rbo_factory.h"

#include "prette/rbo/rbo.h"
#include "prette/rbo/rbo_format.h"

namespace prt::rbo {
  Rbo* RboFactory::CreateRbo(const RboId id) const {
    PRT_ASSERT(IsValidRboId(id));
    Rbo::BindRbo(id);
    Rbo::SetRboStorage(format_, size_);
    Rbo::BindDefaultRbo();
    return Rbo::New(id, format_, size_);
  }

  rx::observable<Rbo*> RboFactory::Create(const int num) const {
    return GenerateRboId(num)
      .map([this](RboId id) {
        return CreateRbo(id);
      });
  }
}