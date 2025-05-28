#include "prette/debug.h"

#include <ostream>

namespace prt {
static const CpuInfo kCpuInfo = cpu_features::GetAarch64Info();

void InitDebug() {}

auto GetCpuInfo() -> CpuInfo const& {
  return kCpuInfo;
}

auto GetCpuFeatures() -> CpuFeatures const& {
  return kCpuInfo.features;
}

void PrintCpuFeatures(const google::LogSeverity severity) {
  const auto& features = GetCpuFeatures();
  CpuFeaturesIterator iter(&features);

  LOG_AT_LEVEL(severity) << "CPU Features:";
  while (iter.HasNext()) {
    const auto [name, enabled] = iter.Next();
    DLOG(INFO) << " - " << name << ": " << enabled;
  }
}
}  // namespace prt