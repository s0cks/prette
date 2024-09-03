#ifndef PRT_RENDERER_H
#define PRT_RENDERER_H

#include "prette/uv/utils.h"
#include "prette/uv/uv_handle.h"

#include "prette/rx.h"
#include "prette/gfx.h"
#include "prette/uv/uv_loop.h"
#include "prette/uv/uv_ticker.h"
#include "prette/relaxed_atomic.h"
#include "prette/render/render_pass.h"
#include "prette/render/render_flags.h"
#include "prette/render/render_events.h"
#include "prette/render/renderer_state.h"
#include "prette/render/renderer_stats.h"

namespace prt {
  namespace engine {
    class TickState;
  }

  namespace render {
    rx::observable<RenderEvent*> OnRenderEvent();
#define DEFINE_ON_RENDER_EVENT(Name)                \
    static inline rx::observable<Name##Event*>      \
    On##Name##Event() {                             \
      return OnRenderEvent()                        \
          .filter(Name##Event::Filter)              \
          .map(Name##Event::Cast);                  \
    }
    FOR_EACH_RENDER_EVENT(DEFINE_ON_RENDER_EVENT)
#undef DEFINE_ON_RENDER_EVENT

    class Renderer : public RenderEventPublisher {
      friend class RenderTicker;
    public:
      static constexpr const auto kDefaultTargetFramesPerSecond = 60;
      static constexpr const auto kTickerRate = NSEC_PER_SEC / 30;

      enum Mode : GLenum {
        kFillMode = GL_FILL,
        kLineMode = GL_LINE,

        kNumberOfModes,
        // aliases
        kDefaultMode = kFillMode,
        kWireframeMode = kLineMode,
      };
    private:
      uv::Loop loop_;
      uv::RateLimitedTicker<kTickerRate> ticker_;
      rx::subscription sub_events_;
      rx::subscription sub_ticker_;
      uv_async_t on_render_;
      RelaxedAtomic<Mode> mode_;
      RenderPass* pass_;

      inline void SetMode(const Mode mode) {
        mode_ = mode;
      }

      void Render();
      static void OnRender(uv_async_t* handle);
    public:
      explicit Renderer(const Mode mode = kDefaultMode);
      virtual ~Renderer();

      Mode GetMode() const {
        return (Mode) mode_;
      }

      RenderPass* GetPass() const {
        return pass_;
      }

      void Run();

      inline void Schedule() {
        const auto status = uv::AsyncSend(&on_render_);
        LOG_IF(ERROR, !status) << "failed to send uv::Async: " << status;
      }

      void Stop() {
        uv::Close(&on_render_);
        loop_.Stop();
      }
    };

    void InitRenderer();
    Renderer* GetRenderer();

    static inline bool
    HasRenderer() {
      return GetRenderer() != nullptr;
    }
  }
  using render::GetRenderer;
}

#endif //PRT_RENDERER_H