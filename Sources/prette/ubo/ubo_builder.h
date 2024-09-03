#ifndef PRT_UBO_BUILDER_H
#define PRT_UBO_BUILDER_H

#include "prette/rx.h"
#include "prette/gfx_usage.h"
#include "prette/ubo/ubo_id.h"

namespace prt::ubo {
  class UboBuilderScope {
    friend class UboBuilderBase;
  protected:
    UboId id_;

    explicit UboBuilderScope(const UboId id);
  public:
    ~UboBuilderScope();

    UboId GetUboId() const {
      return id_;
    }
  };

  class Ubo;
  class UboBuilderBase {
  protected:
    UboBuilderBase() = default;
    void Init(const UboId id) const;
  public:
    virtual ~UboBuilderBase() = default;
    virtual const uint8_t* GetData() const = 0;
    virtual uint64_t GetLength() const = 0;
    virtual uint64_t GetElementSize() const = 0;
    virtual gfx::Usage GetUsage() const = 0;

    uint64_t GetTotalSize() const {
      return GetElementSize() * GetLength();
    }
    
    virtual Ubo* Build() const;
    virtual rx::observable<Ubo*> BuildAsync() const;
  };

  template<typename T>
  class UboBuilder : public UboBuilderBase {
  public:
    static constexpr const auto kElementSize = sizeof(T);
  protected:
    std::vector<T> data_;
    uint64_t length_;
    gfx::Usage usage_;
  public:
    explicit UboBuilder(const uint64_t length = 0, const gfx::Usage usage = gfx::kDefaultUsage):
      UboBuilderBase(),
      data_(),
      length_(length),
      usage_(usage) {
    }
    explicit UboBuilder(const T& value, const gfx::Usage usage = gfx::kDefaultUsage):
      UboBuilder(0, usage) {
      Append(value);
    }
    ~UboBuilder() override = default;

    uint64_t GetElementSize() const override {
      return kElementSize;
    }

    inline bool HasData() const {
      return !data_.empty();
    }

    uint64_t GetLength() const override {
      return HasData()
           ? data_.size()
           : length_;
    }

    const uint8_t* GetData() const override {
      return HasData()
           ? (const uint8_t*) &data_[0]
           : NULL;
    }

    void SetUsage(const gfx::Usage rhs) {
      usage_ = rhs;
    }

    gfx::Usage GetUsage() const override {
      return usage_;
    }

    void Append(const T& value) {
      data_.push_back(value);
    }

    template<typename C>
    void Append(const C& values) {
      data_.insert(std::end(data_), std::begin(values), std::end(values));
    }
  };
}

#endif //PRT_UBO_BUILDER_H