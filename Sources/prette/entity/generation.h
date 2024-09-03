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
    struct EntitySignature {
      EntityId id;
      Signature signature;
    };
  public:
    static constexpr const uint64_t kDefaultGenerationLimit = 65536;
  protected:
    uint64_t id_;
    uint64_t size_;
    uint64_t limit_;
    EntitySignature* entities_;
  public:
    explicit Generation(const uint64_t id,
                        const uint64_t limit = kDefaultGenerationLimit):
      id_(id),
      size_(0),
      limit_(0),
      entities_(nullptr) {
      if(limit <= 0) {
        DLOG(ERROR) << "allocated Generation with " << limit << " limit.";
        return;
      }

      const auto size = RoundUpPow2(limit);
      const auto data = malloc(sizeof(EntitySignature) * size);
      LOG_IF(FATAL, !data) << "failed to allocate Generation of size " << size;
      memset(data, 0, sizeof(data));
      entities_ = (EntitySignature*) data;
      limit_ = size;
    }
    virtual ~Generation() {
      if(entities_)
        free(entities_);
    }

    uint64_t GetGenerationId() const {
      return id_;
    }

    uint64_t GetLimit() const {
      return limit_;
    }

    uint64_t GetSize() const {
      return size_;
    }

    virtual rx::observable<EntityId> GetAll() const;
  };
}

#endif //PRT_GENERATION_H