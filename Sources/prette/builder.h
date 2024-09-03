#ifndef PRT_BUILDER_H
#define PRT_BUILDER_H

#include "prette/rx.h"
#include "prette/metadata.h"

namespace prt {
  class BuilderBase {
  protected:
    Metadata meta_;
  public:
    virtual ~BuilderBase() = default;

    const Metadata& GetMeta() const {
      return meta_;
    }

    void SetMeta(const Metadata& rhs) {
      meta_ = rhs;
    }

    void SetName(const std::string& name) {
      return meta_.SetName(name);
    }

    void Append(const Tag& rhs) {
      return meta_.Append(rhs);
    }

    void Append(const TagSet& rhs) {
      return meta_.Append(rhs);
    }

    void Append(const TagList& rhs) {
      return meta_.Append(rhs);
    }
  };

  template<class T>
  class BuilderTemplate : public BuilderBase {
  protected:
    BuilderTemplate() = default;
  public:
    ~BuilderTemplate() override = default;
    virtual T* Build() const = 0;
    virtual rx::observable<T*> BuildAsync() const = 0;
  };
}

#endif //PRT_BUILDER_H