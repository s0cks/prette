#ifndef PRT_BUILDER_H
#define PRT_BUILDER_H

#include "prette/rx.h"
#include "prette/metadata.h"

namespace prt {
  class BuilderBase {
  private:
    Metadata meta_;
  protected:
    BuilderBase() = default;
  public:
    BuilderBase(const BuilderBase& rhs) = delete;
    BuilderBase(const BuilderBase&& rhs) = delete;
    virtual ~BuilderBase() = default;

    auto GetMeta() const -> const Metadata& {
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

    auto operator=(const BuilderBase&& rhs) -> BuilderBase& = delete;
    auto operator=(const BuilderBase& rhs) -> BuilderBase& = delete;
  };

  template<class T>
  class BuilderTemplate : public BuilderBase {
  protected:
    BuilderTemplate() = default;
  public:
    BuilderTemplate(const BuilderTemplate<T>&& rhs) = delete;
    BuilderTemplate(const BuilderTemplate<T>& rhs) = delete;
    ~BuilderTemplate() override = default;
    virtual auto Build() const -> T* = 0;
    virtual auto BuildAsync() const -> rx::observable<T*> {
      return rx::observable<>::create<T*>([this](rx::subscriber<T*> s) {
        const auto result = Build();
        if(!result)
          return s.on_error(rx::util::make_error_ptr(std::runtime_error("failed to build.")));
        s.on_next(result);
        s.on_completed();
      });
    }

    auto operator=(const BuilderTemplate<T>&& rhs) -> BuilderTemplate<T>& = delete;
    auto operator=(const BuilderTemplate<T>& rhs) -> BuilderTemplate<T>& = delete;
  };
}

#endif //PRT_BUILDER_H