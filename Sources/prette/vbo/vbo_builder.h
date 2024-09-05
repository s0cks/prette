#ifndef PRT_VBO_FACTORY_H
#define PRT_VBO_FACTORY_H

#include "prette/class.h"
#include "prette/region.h"
#include "prette/gfx_usage.h"
#include "prette/vbo/vbo_id.h"

namespace prt::vbo {
  class Vbo;
  class VboBuilderBase {
  protected:
    Class* class_;
    gfx::Usage usage_;

    explicit VboBuilderBase(Class* cls):
      class_(cls),
      usage_(gfx::kDynamicDrawUsage) {
      PRT_ASSERT(cls);
    }
    
    virtual Region GetSource() const = 0;
  public:
    virtual ~VboBuilderBase() = default;
    virtual uword GetLength() const = 0;

    Class* GetClass() const {
      return class_;
    }

    gfx::Usage GetUsage() const {
      return usage_;
    }

    void SetUsage(const gfx::Usage rhs) {
      usage_ = rhs;
    }

    inline uword GetVertexSize() const {
      return GetClass()->GetAllocationSize();
    }

    inline uword GetTotalSize() const {
      return GetVertexSize() * GetLength();
    }

    virtual Vbo* Build() const;
    rx::observable<Vbo*> BuildAsync() const {
      return rx::observable<>::create<Vbo*>([this](rx::subscriber<Vbo*> s) {
        const auto value = Build();
        if(!value) {
          const auto err = "failed to build Vbo.";
          return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
        }

        s.on_next(value);
        s.on_completed();
      });
    }
  };

  template<class V>
  class VboBuilder : public VboBuilderBase {
  public:
    typedef std::vector<V> VertexList;
  private:
    VertexList data_;

    Region GetSource() const override {
      const auto address = !data_.empty()
        ? ((uword) &data_[0])
        : 0;
      return Region(address, GetTotalSize());
    }
  public:
    explicit VboBuilder(Class* cls):
      VboBuilderBase(cls),
      data_() {
    }
    VboBuilder(Class* cls, const VertexList& data):
      VboBuilderBase(cls),
      data_(data) {
    }
    VboBuilder(Class* cls, const uword length):
      VboBuilderBase(cls),
      data_() {
      data_.reserve(length);
    }
    ~VboBuilder() override = default;

    uword GetLength() const override {
      return data_.empty() ? data_.capacity() : data_.size();
    }

    void SetLength(const uword length) {
      PRT_ASSERT(length >= 0);
      PRT_ASSERT(length >= data_.capacity());
      data_.reserve(length);
    }

    void Append(const VertexList& data) {
      data_.insert(std::end(data_), std::begin(data), std::end(data));
    }

    void Append(const V& value) {
      data_.push_back(value);
    }

    void Append(const rx::observable<V>& data) {
      data.as_blocking()
        .subscribe([this](const V& value) {
          return Append(value);
        });
    }

    void SetData(const VertexList& rhs) {
      data_.clear();
      Append(rhs);
    }

    void SetData(const rx::observable<V>& rhs) {
      data_.clear();
      Append(rhs);
    }
  };
}

#endif //PRT_VBO_FACTORY_H