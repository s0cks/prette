#ifndef PRT_GFX_MAPPED_BUFFER_SCOPE_H
#define PRT_GFX_MAPPED_BUFFER_SCOPE_H

#include "prette/gfx.h"
#include "prette/region.h"
#include "prette/platform.h"
#include "prette/gfx_access.h"

namespace prt::gfx {
  class MappedBufferScope : public Region {
    DEFINE_NON_COPYABLE_TYPE(MappedBufferScope);

    template<const GLenum Target, const Access A>
    friend class MappedBufferScopeTemplate;
  public:
    template<typename T>
    class Iterator {
    protected:
      const MappedBufferScope* scope_;
      uword current_;

      inline const MappedBufferScope* scope() const {
        return scope_;
      }
    public:
      explicit Iterator(const MappedBufferScope* scope):
        scope_(scope),
        current_(scope->GetStartingAddress()) {
      }
      ~Iterator() = default;

      bool HasNext() const {
        return current_ <= scope()->GetEndingAddress();
      }

      T* Next() {
        const auto next = (T*)current_;
        current_ += sizeof(T);
        return next;
      }
    };
  private:
    static void UnmapBuffer(const GLenum target);
    static uword MapBuffer(const GLenum target, const Access access);
  protected:
    MappedBufferScope() = default;
  public:
    ~MappedBufferScope() override = default;

    virtual GLenum GetTarget() const = 0;
    virtual Access GetAccess() const = 0;

    bool IsReadOnly() const {
      return GetAccess() == kReadOnly;
    }

    bool IsWriteOnly() const {
      return GetAccess() == kWriteOnly;
    }

    bool IsReadWrite() const {
      return GetAccess() == kReadWrite;
    }

    bool IsMapped() const {
      return GetStartingAddress() != UNALLOCATED
          && GetSize() > 0;
    }

    void Release() {
      PRT_ASSERT(IsMapped());
      return UnmapBuffer(GetTarget());
    }
  };

  template<const GLenum Target, const Access A>
  class MappedBufferScopeTemplate : public MappedBufferScope {
    DEFINE_NON_COPYABLE_TYPE(MappedBufferScopeTemplate);
  protected:
    explicit MappedBufferScopeTemplate(const uword size):
      MappedBufferScope() {
      const auto address = MappedBufferScope::MapBuffer(GetTarget(), GetAccess());
      if(address == UNALLOCATED) {
        DLOG(ERROR) << "failed to map buffer."; //TODO: better error handling.
        return;
      }
      start_ = address;
      size_ = size;
    }
  public:
    ~MappedBufferScopeTemplate() override {
      if(IsMapped())
        Release();
    }

    GLenum GetTarget() const override {
      return Target;
    }

    Access GetAccess() const override {
      return A;
    }
  };

  template<const GLenum Target>
  class WriteOnlyMappedBufferScope : public MappedBufferScopeTemplate<Target, kWriteOnly> {
    DEFINE_NON_COPYABLE_TYPE(WriteOnlyMappedBufferScope);
  public:
    explicit WriteOnlyMappedBufferScope(const uword size):
      MappedBufferScopeTemplate<Target, kWriteOnly>(size) {
    }
    ~WriteOnlyMappedBufferScope() override = default;
  };

  template<const GLenum Target>
  class ReadOnlyMappedBufferScope : public MappedBufferScopeTemplate<Target, kReadOnly> {
    DEFINE_NON_COPYABLE_TYPE(ReadOnlyMappedBufferScope);
  public:
    explicit ReadOnlyMappedBufferScope(const uword size):
      MappedBufferScopeTemplate<Target, kReadOnly>(size) {
    }
    ~ReadOnlyMappedBufferScope() override = default;
  };
}

#endif //PRT_GFX_MAPPED_BUFFER_SCOPE_H