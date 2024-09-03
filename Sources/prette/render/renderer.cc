#include "prette/render/renderer.h"

#include "prette/thread_local.h"
#include "prette/engine/engine.h"
#include "prette/render/renderer_stats.h"
#include "prette/render/render_pass_root.h"
#include "prette/render/render_pass_executor.h"

namespace prt::render {
  static RenderEventSubject events_;
  static ThreadLocal<Renderer> renderer_;

  Renderer::Renderer(const Mode mode):
    loop_(),
    ticker_(&loop_),
    on_render_(),
    sub_events_(),
    sub_ticker_(),
    mode_(mode),
    pass_(new RootRenderPass()) {
    sub_events_ = OnEvent()
      .subscribe(render::events_.get_subscriber());
    sub_ticker_ = ticker_.ToObservable()
      .subscribe([this](const uv::Tick& tick) {
        Schedule();
      });

    uv::SetHandleData(on_render_, this);
    {
      const auto status = uv::InitAsync(loop_, &on_render_, &OnRender);
      LOG_IF(ERROR, !status) << "failed to initialize uv::Async: " << status;
    }
  }

  Renderer::~Renderer() {
    sub_events_.unsubscribe();
    sub_ticker_.unsubscribe();
  }

  void Renderer::Render() {
    PRT_ASSERT(pass_);
    Publish<PreRenderEvent>();
    RenderPassExecutor::Execute(pass_);
    Publish<PostRenderEvent>(ticker_.GetCurrentTick());
  }

  void Renderer::OnRender(uv_async_t* handle) {
    const auto renderer = uv::GetHandleData<Renderer>(handle);
    PRT_ASSERT(renderer);
    renderer->Render();
  }

  void Renderer::Run() {
    loop_.RunNoWait();
  }

  rx::observable<RenderEvent*> OnRenderEvent() {
    return events_.get_observable();
  }

  static inline void
  Publish(RenderEvent* event) {
    PRT_ASSERT(event);
    const auto& subscriber = events_.get_subscriber();
    subscriber.on_next(event);
  }

  template<class E, typename... Args>
  static inline void
  Publish(Args... args) {
    E event(args...);
    return Publish((RenderEvent*) &event);
  }

  static inline void
  SetRenderer(Renderer* renderer) {
    PRT_ASSERT(renderer);
    renderer_.Set(renderer);
  }

  Renderer* GetRenderer() {
    return renderer_.Get();
  }

  void InitRenderer() {
    LOG_IF(FATAL, GetRenderer() != nullptr) << "cannot reinitialize the Renderer.";
    DLOG(INFO) << "initializing Renderer....";
    const auto renderer = new Renderer();
    SetRenderer(renderer);
    Publish<RendererInitializedEvent>(renderer);
  }
}