#ifndef PRT_RENDER_SETTINGS_H
#define PRT_RENDER_SETTINGS_H

#include "prette/resolution.h"
#include "prette/settings/setting.h"
#include "prette/render/render_flags.h"

namespace prt::render {
  class ResolutionSetting : public settings::SettingTemplate<Resolution> {
  public:
    explicit ResolutionSetting(const Resolution& init_value):
      SettingTemplate(init_value) {
    }
    ~ResolutionSetting() override = default;
    DECLARE_SETTING(Resolution);
  public:
    static inline ResolutionSetting*
    New(const Resolution& init_value) {
      return new ResolutionSetting(init_value);
    }
  };

  ResolutionSetting* GetResolutionSetting();

  static inline Resolution
  GetResolution() {
    const auto res = GetResolutionSetting();
    PRT_ASSERT(res);
    return res->GetValue();
  }
}

#endif //PRT_RENDER_SETTINGS_H