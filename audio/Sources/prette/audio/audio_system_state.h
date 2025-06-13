#ifndef PRT_AUDIO_SYSTEM_STATE_H
#define PRT_AUDIO_SYSTEM_STATE_H

#include <cstdint>

namespace prt::audio {
#define FOR_EACH_AUDIO_SYSTEM_STATE(V) \
  V(AudioSystemCreated)                \
  V(AudioSystemInit)                   \
  V(AudioSystemDestroyed)

enum AudioSystemState : uint8_t {
#define DEFINE_STATE(Name) k##Name,
  FOR_EACH_AUDIO_SYSTEM_STATE(DEFINE_STATE)
#undef DEFINE_STATE
};
}  // namespace prt::audio

#endif  // PRT_AUDIO_SYSTEM_STATE_H
