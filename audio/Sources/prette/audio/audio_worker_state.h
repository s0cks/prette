#ifndef PRT_AUDIO_WORKER_STATE_H
#define PRT_AUDIO_WORKER_STATE_H

#include <iostream>

namespace prt::audio {
#define FOR_EACH_AUDIO_WORKER_STATE(V) \
  V(Stopped)                           \
  V(Starting)                          \
  V(Running)                           \
  V(Stopping)                          \
  V(Error)

enum AudioWorkerState {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_AUDIO_WORKER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};

static inline auto operator<<(std::ostream& stream, const AudioWorkerState& rhs) -> std::ostream& {
  switch (rhs) {
#define DEFINE_TOSTRING(Name)     \
  case AudioWorkerState::k##Name: \
    return stream << #Name;

    FOR_EACH_AUDIO_WORKER_STATE(DEFINE_TOSTRING)
#undef DEFINE_TOSTRING
  }
}
}  // namespace prt::audio

#endif  // PRT_AUDIO_WORKER_STATE_H
