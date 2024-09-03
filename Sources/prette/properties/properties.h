#ifndef PRT_PROPERTIES_H
#define PRT_PROPERTIES_H

#include "prette/properties/property.h"
#include "prette/properties/property_set.h"
#include "prette/properties/properties_parser.h"

#ifdef PRT_DEBUG

#include "prette/properties/property_set_printer.h"

#endif //PRT_DEBUG

namespace prt::properties {
  static inline bool
  Parse(FILE* file, PropertySet& props) {
    PRT_ASSERT(file);
    Parser parser(file);
    return parser.ParseProperties(props);
  }

  static inline bool
  Parse(const uri::Uri& uri, PropertySet& props) {
    PRT_ASSERT(uri.HasScheme("file"));
    PRT_ASSERT(uri.HasExtension("properties"));
    if(!StartsWith(uri.path, FLAGS_resources)) {
      const auto normalized = uri.ToFileUri(FLAGS_resources); // TODO: prolly should change ToFileUri to something like Normalize, considering it is already a file uri
      return Parse(normalized, props);
    }
    const auto file = uri.OpenFileForReading();
    if(!file) {
      LOG(ERROR) << "failed to open: " << uri;
      return false;
    }
    return Parse(file, props);
  }
}

#endif //PRT_PROPERTIES_H