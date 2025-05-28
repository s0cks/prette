#ifndef PRT_DEBUG_H
#define PRT_DEBUG_H

#ifdef PRT_DEBUG

#include <glog/logging.h>
#include <string>
#include <utility>

#include "prette/platform.h"
#ifdef ARCH_IS_ARM64

// IWYU pragma: begin_exports
#include <cpuinfo_aarch64.h>
// IWYU pragma: end_exports

#else
#error "Unsupported CPU architecture"
#endif

namespace prt {
#ifdef ARCH_IS_ARM64
using CpuInfo = cpu_features::Aarch64Info;
using CpuFeatures = cpu_features::Aarch64Features;
using CpuFeaturesEnum = cpu_features::Aarch64FeaturesEnum;

static constexpr const auto kTotalNumberOfCpuFeatures = cpu_features::AARCH64_LAST_;

static inline auto GetCpuFeature(const CpuFeatures* features, const CpuFeaturesEnum feature)
    -> std::pair<std::string, int> {
  const auto name = cpu_features::GetAarch64FeaturesEnumName(feature);
  const auto value = cpu_features::GetAarch64FeaturesEnumValue(features, feature);
  return std::make_pair(name, value);
}

#else
#error "Unsupported CPU Architecture"
#endif

class CpuFeaturesIterator {
 private:
  const CpuFeatures* features_;
  int current_ = 0;

 public:
  explicit CpuFeaturesIterator(const CpuFeatures* features) :
    features_(features) {}
  ~CpuFeaturesIterator() = default;

  inline auto features() const -> const CpuFeatures* {
    return features_;
  }

  inline auto current_feature_idx() const -> int {
    return current_;
  }

  inline auto current_feature() const -> CpuFeaturesEnum {
    return static_cast<CpuFeaturesEnum>(current_feature_idx());
  }

  auto HasNext() const -> bool {
    return current_ < kTotalNumberOfCpuFeatures;
  }

  auto Next() -> std::pair<std::string, int> {
    const auto next = GetCpuFeature(features(), current_feature());
    current_ += 1;
    return std::move(next);
  }
};

void InitDebug();
auto GetCpuInfo() -> CpuInfo const&;
auto GetCpuFeatures() -> CpuFeatures const&;
void PrintCpuFeatures(const google::LogSeverity severity = google::INFO);
}  // namespace prt

#endif  // PRT_DEBUG
#endif  // PRT_DEBUG_H
