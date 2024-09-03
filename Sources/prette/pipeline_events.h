#ifndef PRT_PIPELINE_EVENTS_H
#define PRT_PIPELINE_EVENTS_H

#include "prette/event.h"
#include "prette/common.h"

namespace prt::pipeline {
#define FOR_EACH_PIPELINE_EVENT(V)        \
  V(PipelineStarted)                      \
  V(PipelineFinished)

  class Pipeline;
  class PipelineEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
  FOR_EACH_PIPELINE_EVENT(FORWARD_DECLARE)

  class PipelineEvent : public Event {
  protected:
    const Pipeline* pipeline_;

    explicit PipelineEvent(const Pipeline* pipeline):
      Event(),
      pipeline_(pipeline) {
    }
  public:
    ~PipelineEvent() override = default;

    const Pipeline* GetPipeline() const {
      return pipeline_;
    }

    DEFINE_EVENT_PROTOTYPE(FOR_EACH_PIPELINE_EVENT);
  };

#define DECLARE_PIPELINE_EVENT(Name)                                \
  DECLARE_EVENT_TYPE(PipelineEvent, Name)

  class PipelineStartedEvent : public PipelineEvent {
  public:
    explicit PipelineStartedEvent(const Pipeline* pipeline):
      PipelineEvent(pipeline) {
    }
    ~PipelineStartedEvent() override = default;
    DECLARE_PIPELINE_EVENT(PipelineStarted);
  };

  class PipelineFinishedEvent : public PipelineEvent {
  public:
    explicit PipelineFinishedEvent(const Pipeline* pipeline):
      PipelineEvent(pipeline) {
    }
    ~PipelineFinishedEvent() override = default;
    DECLARE_PIPELINE_EVENT(PipelineFinished);
  };
}

#endif //PRT_PIPELINE_EVENTS_H