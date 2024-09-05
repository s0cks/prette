#ifndef PRT_GFX_BUFFER_H
#define PRT_GFX_BUFFER_H

#include "prette/class.h"
#include "prette/region.h"
#include "prette/gfx_usage.h"
#include "prette/gfx_object.h"

namespace prt {
  namespace gfx {
    template<typename Id, const GLenum Target>
    class BufferObject : public Object<Id> {
    protected:
      Class* class_;
      uword length_;
      Usage usage_;

      BufferObject() = default;
      BufferObject(const Id id, Class* cls, const uword length, const Usage usage):
        Object<Id>(id),
        class_(cls),
        length_(length), 
        usage_(usage) {
        PRT_ASSERT(cls);
        PRT_ASSERT(length >= 0);
      }

      void SetLength(const uword length) {
        PRT_ASSERT(length >= 0);
        length_ = length;
      }
    protected:
      static inline void
      Bind(const Id id) {
        PRT_ASSERT(id.IsValid());
        glBindBuffer(Target, id);
        CHECK_GL;
      }

      static inline void
      InitData(const Region& src, const gfx::Usage usage) {
        glBufferData(Target, src.GetSize(), (const GLvoid*) src, usage);
        CHECK_GL;
      }

      static inline void
      PutData(const uword offset, const Region& src) {
        glBufferSubData(Target, (GLintptr) offset, (GLsizeiptr) src.GetSize(), (const GLvoid*) src);
        CHECK_GL;
      }

      static inline void
      PutData(const Region& src) {
        return PutData(0, src);
      }

      static inline void
      Delete(const Id* ids, const int num_ids) {
        PRT_ASSERT(num_ids >= 1);
        glDeleteBuffers(num_ids, (const GLuint*) &ids[0]);
        CHECK_GL;
      }

      static inline void
      Delete(const Id id) {
        return Delete(&id, 1);
      }
    public:
      ~BufferObject() override = default;

      Class* GetClass() const {
        return class_;
      }

      uword GetElementSize() const {
        return GetClass()->GetAllocationSize();
      }

      uword GetLength() const {
        return length_;
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