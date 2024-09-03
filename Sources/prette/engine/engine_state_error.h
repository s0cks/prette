#ifndef PRT_ENGINE_STATE_H
#error "Please #include <prette/engine/engine_state.h> instead."
#endif //PRT_ENGINE_STATE_H

#ifndef PRT_ENGINE_STATE_ERROR_H
#define PRT_ENGINE_STATE_ERROR_H

namespace prt::engine {
  class Engine;
  class ErrorState : public EngineState {
    friend class Engine;
  protected:
    explicit ErrorState(Engine* engine);
  public:
    ~ErrorState() override;
    DECLARE_ENGINE_STATE(Error);
  };
}

#endif //PRT_ENGINE_STATE_ERROR_H