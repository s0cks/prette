#ifndef PRT_AUDIO_EVENT_H
#define PRT_AUDIO_EVENT_H

#include "prette/event.h"

namespace prt::audio {
#define FOR_EACH_AUDIO_EVENT(V)

class AudioEvent;
#define FORWARD_DECLARE(Name) class Name##Event;
FOR_EACH_AUDIO_EVENT(FORWARD_DECLARE)
#undef FORWARD_DECLARE

class AudioEvent : public Event {
 public:
  AudioEvent() = default;
  ~AudioEvent() override = default;
  DEFINE_EVENT_PROTOTYPE(AudioEvent, FOR_EACH_AUDIO_EVENT);
};

DEFINE_EVENT_OBSERVABLE(Audio);
DEFINE_EVENT_SUBJECT(Audio);
}  // namespace prt::audio

#endif  // PRT_AUDIO_EVENT_H
