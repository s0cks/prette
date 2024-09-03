#include "prette/json_flags.h"

namespace prt::json {
DEFINE_string(json_schema_path, kDefaultSchemaPath, "The path for the JSON schema file.");
DEFINE_bool(json_schema_validation_enabled, kDefaultSchemaValidationEnabled, "Enable JSON schema validation.");
}