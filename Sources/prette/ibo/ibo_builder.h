#ifndef PRT_IBO_BUILDER_H
#define PRT_IBO_BUILDER_H

#include "prette/rx.h"
#include "prette/region.h"
#include "prette/ibo/ibo.h"

namespace prt {
  namespace ibo {
    class IboBuilderBase {
    protected:
      Class* class_;
      gfx::Usage usage_;

      virtual Region GetSource() const = 0;
    public:
      explicit IboBuilderBase(Class* cls):
        class_(cls),
        usage_(Ibo::kDefaultUsage) {
        PRT_ASSERT(cls);
      }
      virtual ~IboBuilderBase() = default;
      virtual uword GetLength() const = 0;

      Class* GetClass() const {
        return class_;
      }

      gfx::Usage GetUsage() const {
        return usage_;
      }

      void SetUsage(const gfx::Usage& rhs) {
        usage_ = rhs;
      }

      inline uword GetElementSize() const {
        return GetClass()->GetAllocationSize();
      }

      inline uword GetTotalSize() const {
        return GetElementSize() * GetLength();
      }

      virtual Ibo* Build() const;
      rx::observable<Ibo*> BuildAsync() const {
        return rx::observable<>::create<Ibo*>([this](rx::subscriber<Ibo*> s) {
          const auto value = Build();
          if(!value) {
            const auto err = "failed to build Ibo.";
            return s.on_error(rx::util::make_error_ptr(std::runtime_error(err)));
          }

          s.on_next(value);
          s.on_completed();
        });
      }
    };

    template<typename I>
    class IboBuilder : public IboBuilderBase {
    public:
      typedef std::vector<I> IndexList;
    protected:
      IndexList data_;

      explicit IboBuilder(Class* cls, const IndexList& data):
        IboBuilderBase(cls),
        data_(data) {
        PRT_ASSERT(sizeof(I) == cls->GetAllocationSize());
      }

      Region GetSource() const override {
        if(data_.empty())
          return Region();
        return Region((uword) &data_[0], GetTotalSize());
      }
    public:
      ~IboBuilder() override = default;

      uword GetLength() const override {
        return data_.size();
      }

      void Append(const IndexList& data) {
        data_.insert(std::end(data_), std::begin(data), std::end(data));
      }

      void Append(const I& value) {
        data_.push_back(value);
      }

      void Append(const rx::observable<I>& data) {
        data.as_blocking()
          .subscribe([this](const I& value) {
            return Append(value);
          });
      }

      void SetData(const IndexList& rhs) {
        data_.clear();
        Append(rhs);
      }

      void SetData(const rx::observable<I>& rhs) {
        data_.clear();
        Append(rhs);
      }
    };

    class ByteIboBuilder : public IboBuilder<int8_t> {
    public:
      explicit ByteIboBuilder(const IndexList& data = {}):
        IboBuilder<int8_t>(Class::kByte, data) {
      }
      ~ByteIboBuilder() override = default;
    };

    class UByteIboBuilder : public IboBuilder<uint8_t> {
    public:
      explicit UByteIboBuilder(const IndexList& data = {}):
        IboBuilder<uint8_t>(Class::kByte, data) {
      }
      ~UByteIboBuilder() override = default;
    };

    class ShortIboBuilder : public IboBuilder<int16_t> {
    public:
      explicit ShortIboBuilder(const IndexList& data = {}):
        IboBuilder<int16_t>(Class::kShort, data) {
      }
      ~ShortIboBuilder() override = default;
    };

    class UShortIboBuilder : public IboBuilder<uint16_t> {
    public:
      explicit UShortIboBuilder(const IndexList& data = {}):
        IboBuilder<uint16_t>(Class::kUnsignedShort, data) {
      }
      ~UShortIboBuilder() override = default;
    };

    class IntIboBuilder : public IboBuilder<int32_t> {
    public:
      explicit IntIboBuilder(const IndexList& data = {}):
        IboBuilder<int32_t>(Class::kInt, data) {
      }
      ~IntIboBuilder() override = default;
    };

    class UIntIboBuilder : public IboBuilder<uint32_t> {
    public:
      explicit UIntIboBuilder(const IndexList& data = {}):
        IboBuilder<uint32_t>(Class::kUnsignedInt, data) {
      }
      ~UIntIboBuilder() override = default;
    };
  }
}

#endif //PRT_IBO_BUILDER_H