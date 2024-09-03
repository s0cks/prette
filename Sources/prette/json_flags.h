#ifndef PRT_JSON_FLAGS_H
#define PRT_JSON_FLAGS_H

#include <fmt/format.h>

#include "prette/uri.h"
#include "prette/flags.h"
#include "prette/common.h"

namespace prt::json {
static constexpr const auto kDefaultSchemaPath = "mcc.schema.json";
DECLARE_string(json_schema_path);

#ifdef PRT_DEBUG
static constexpr const auto kDefaultSchemaValidationEnabled = true;
#else 
static constexpr const auto kDefaultSchemaValidationEnabled = false;
#endif //PRT_DEBUG
DECLARE_bool(json_schema_validation_enabled);

  static inline std::string
  GetSchemaPath() {
    auto path = FLAGS_json_schema_path;
    if(!StartsWith(path, "/"))
      path = fmt::format("/{0:s}", path);
    if(!StartsWith(path, FLAGS_resources))
      path = fmt::format("{0:s}{1:s}", FLAGS_resources, path);
    return path;
  }

  static inline uri::Uri
  GetSchemaUri() {
    return uri::Uri(fmt::format("file://{0:s}", GetSchemaPath()));
  }

  static inline bool
  IsSchemaValidationEnabled() {
    return FLAGS_json_schema_validation_enabled;
  }
}

#endif //PRT_JSON_FLAGS_H