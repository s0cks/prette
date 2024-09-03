#ifndef PRT_VBO_FACTORY_H
#define PRT_VBO_FACTORY_H

#include "prette/region.h"
#include "prette/vbo/vbo.h"

namespace prt::vbo {
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
    std::vector<V> data_;
    uword length_;

    Region GetSource() const override {
      if(data_.empty())
        return Region(0, GetTotalSize());
      return Region((uword) &data_[0], GetTotalSize());
    }
  public:
    VboBuilder(Class* cls,
               const VertexList& data = {}):
      VboBuilderBase(cls),
      data_(data),
      length_(0) {
    }
    ~VboBuilder() override = default;

    uword GetLength() const override {
      return !data_.empty() ? data_.size() : length_;
    }

    void SetLength(const uword length) {
      PRT_ASSERT(length >= 0);
      length_ = length;
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