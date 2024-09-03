#ifndef PRT_RBO_FACTORY_H
#define PRT_RBO_FACTORY_H

#include "prette/rx.h"
#include "prette/rbo/rbo_id.h"
#include "prette/rbo/rbo_size.h"
#include "prette/rbo/rbo_format.h"

namespace prt::rbo {
  class Rbo;
  class RboFactory {
  protected:
    RboFormat format_;
    RboSize size_;

    virtual Rbo* CreateRbo(const RboId id) const;
  public:
    RboFactory(const RboFormat format,
               const RboSize& size):
      format_(format),
      size_(size) {
    }
    ~RboFactory() = default;
    virtual rx::observable<Rbo*> Create(const int num = 1) const;
  };
}

#endif //PRT_RBO_FACTORY_H