#ifndef PRT_RBO_H
#define PRT_RBO_H

#include "prette/gfx_object.h"
#include "prette/rbo/rbo_id.h"
#include "prette/rbo/rbo_size.h"
#include "prette/rbo/rbo_events.h"
#include "prette/rbo/rbo_format.h"

namespace prt {
  namespace rbo {
    rx::observable<RboEvent*> OnRboEvent();
    rx::observable<RboEvent*> OnRboEvent(const RboId id);

#define DEFINE_ON_RBO_EVENT(Name) \
    static inline rx::observable<Name##Event*>      \
    On##Name##Event() {                             \
      return OnRboEvent()                           \
        .filter(Name##Event::Filter)                \
        .map(Name##Event::Cast);                    \
    }                                               \
    static inline rx::observable<Name##Event*>      \
    On##Name##Event(const RboId id) {               \
      return OnRboEvent()                           \
        .filter(Name##Event::FilterBy(id))          \
        .map(Name##Event::Cast);                    \
    }
    FOR_EACH_RBO_EVENT(DEFINE_ON_RBO_EVENT)
#undef DEFINE_ON_RBO_EVENT

    class RboFactory;
    class Rbo : public gfx::Object<RboId> {
      friend class RboFactory;
    public:
      static int32_t GetMaxSize();
    private:
      static void PublishEvent(RboEvent* event);
      static void BindRbo(const RboId id);
      static void SetRboStorage(const RboFormat format, const RboSize& size);
      
      static inline void
      BindDefaultRbo() {
        return BindRbo(kDefaultRboId);
      }

      template<class E, typename... Args>
      static inline void
      PublishEvent(const Rbo* rbo, Args... args) {
        E event(rbo, args...);
        return PublishEvent((RboEvent*) &event);
      }
    protected:
      RboFormat format_;
      RboSize size_;

      explicit Rbo(const RboId id,
                   const RboFormat format,
                   const RboSize& size):
        Object<RboId>(id),
        format_(format),
        size_(size) {
      }

      template<class E, typename... Args>
      inline void
      Publish(Args... args) const {
        return PublishEvent<E>(this, args...);
      }
    public:
      ~Rbo() override = default;

      RboFormat GetFormat() const {
        return format_;
      }

      RboSize GetSize() const {
        return size_;
      }

      inline uint32_t GetWidth() const {
        return size_[0];
      }

      inline uint32_t GetHeight() const {
        return size_[1];
      }

      inline rx::observable<RboEvent*> OnEvent() const {
        return OnRboEvent(GetId());
      }
      
      std::string ToString() const override;

#define DEFINE_ON_RBO_EVENT(Name)                                           \
      inline rx::observable<Name##Event*> On##Name##Event() const {         \
        return rbo::On##Name##Event(GetId());                               \
      }
      FOR_EACH_RBO_EVENT(DEFINE_ON_RBO_EVENT)
#undef DEFINE_RBO_EVENT
    private:
      static Rbo* New(const RboId id, const RboFormat format, const RboSize& size);
    public:
      static Rbo* New(const RboFormat format, const RboSize& size);

#define DEFINE_NEW_RBO_FORMAT(Name, GlValue)              \
      static inline Rbo*                                  \
      New##Name(const RboSize& size) {                    \
        return New(k##Name##Format, size);                \
      }
      FOR_EACH_RBO_FORMAT(DEFINE_NEW_RBO_FORMAT)
#undef DEFINE_NEW_RBO_FORMAT
    };
  }
  using rbo::Rbo;
}

#endif //PRT_RBO_H