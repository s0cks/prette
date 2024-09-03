#ifndef PRT_GFX_BUFFER_H
#define PRT_GFX_BUFFER_H


#include "prette/gfx_usage.h"
#include "prette/gfx_object.h"

namespace prt {
  namespace gfx {
    template<typename Id>
    class BufferObject : public Object {
    protected:
      Id id_;
      Usage usage_;

      BufferObject(const Id id, const Usage usage):
        Object(),
        id_(id),
        usage_(usage) {
      }
    public:
      ~BufferObject() override = default;
      virtual uword GetElementSize() const = 0;
      virtual uword GetLength() const = 0;

      Id GetId() const {
        return id_;
      }
      
      Usage GetUsage() const {
        return usage_;
      }

      uword GetTotalSize() const {
        return GetElementSize() * GetLength();
      }

      inline bool IsEmpty() const {
        return GetLength() == 0;
      }

      inline bool IsStreamUsage() const {
        return gfx::IsStreamUsage(GetUsage());
      }

      inline bool IsDynamicUsage() const {
        return gfx::IsDynamicUsage(GetUsage());
      }

      inline bool IsStaticUsage() const {
        return gfx::IsStaticUsage(GetUsage());
      }

#define DEFINE_USAGE_TYPE_CHECK(Name, _)              \
      inline bool Is##Name##Usage() const {           \
        return GetUsage() == gfx::k##Name##Usage;     \
      }
      FOR_EACH_GFX_USAGE(DEFINE_USAGE_TYPE_CHECK)
#undef DEFINE_USAGE_TYPE_CHECK
    };
  }
}

#endif //PRT_GFX_BUFFER_H