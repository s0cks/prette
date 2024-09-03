#ifndef PRT_SETTINGS_H
#define PRT_SETTINGS_H

#include <optional>
#include <leveldb/db.h>
#include <gflags/gflags.h>

#include "prette/gfx.h"
#include "prette/common.h"

#ifndef PRT_SETTINGS_SYNC_WRITES
#define PRT_SETTINGS_SYNC_WRITES false
#endif //PRT_SETTINGS_SYNC_WRITES

namespace prt::settings {
  DECLARE_string(settings_dir);

  static constexpr const auto kSyncWrites = PRT_SETTINGS_SYNC_WRITES;

  void Init();

  std::optional<std::string> GetString(const std::string& name);
  bool PutString(const std::string& name, const std::string& value);
  
  std::optional<glm::vec2> GetVec2(const std::string& name);
  bool PutVec2(const std::string& name, const glm::vec2& value);

  std::optional<glm::vec3> GetVec3(const std::string& name);
  bool PutVec3(const std::string& name, const glm::vec3& value);
}

#endif //PRT_SETTINGS_H