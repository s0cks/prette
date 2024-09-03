#ifndef PRT_SETTINGS_DB_H
#define PRT_SETTINGS_DB_H

#include "prette/platform.h"

namespace prt::settings {
  class SettingsKey {
  private:
    enum Layout {

    };
    
    uint8_t data_;
  public:
    SettingsKey() = default;
    SettingsKey(const SettingsKey& rhs) = default;
    ~SettingsKey() = default;
    SettingsKey& operator=(const SettingsKey& rhs) = default;
  };

  class SettingsStorage {
  protected:
  public:
    virtual ~SettingsStorage() = default;
  };
}

#endif //PRT_SETTINGS_DB_H