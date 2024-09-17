#ifndef PRT_GENERATION_H
#define PRT_GENERATION_H

#include <cstdint>
#include <glog/logging.h>

#include "prette/rx.h"
#include "prette/common.h"
#include "prette/entity/entity_id.h"
#include "prette/entity/entity_signature.h"

namespace prt::entity {
  class Generation {
    struct EntitySignature { //NOLINT(cppcoreguidelines-pro-type-member-init)
      EntityId id;
      Signature signature;
    };
  public:
    static constexpr const uint64_t kDefaultGenerationLimit = 65536;
  private:
    uint64_t id_;
    uint64_t size_{};
    uint64_t limit_{};
    EntitySignature* entities_{};
  public:
    explicit Generation(const uint64_t id,
                        const uint64_t limit = kDefaultGenerationLimit):
      id_(id) {
      if(limit <= 0) {
        DLOG(ERROR) << "allocated Generation with " << limit << " limit.";
        return;
      }

      const auto size = RoundUpPow2(limit);
      const auto total_size = sizeof(EntitySignature) * size;
      const auto data = malloc(total_size);
      LOG_IF(FATAL, !data) << "failed to allocate Generation of size " << size;
      memset(data, 0, total_size);
      entities_ = (EntitySignature*) data; //NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
      limit_ = size;
    }
    virtual ~Generation() {
      if(entities_)
        free(entities_);
    }

    auto GetGenerationId() const -> uint64_t {
      return id_;
    }

    auto GetLimit() const -> uint64_t {
      return limit_;
    }

    auto GetSize() const -> uint64_t {
      return size_;
    }

    virtual auto GetAll() const -> rx::observable<EntityId>;
  };
}

#endif //PRT_GENERATION_H