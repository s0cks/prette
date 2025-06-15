#include "prette/city_storage.h"

#include <algorithm>
#include <deque>
#include <filesystem>
#include <flatbuffers/flatbuffer_builder.h>
#include <fmt/format.h>
#include <fstream>
#include <ios>
#include <re2/re2.h>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/city.h"
#include "prette/common.h"

namespace prt {
CityStorage::CityStorage(const fs::path root) :
  root_(std::move(root)) {
  if (!fs::exists(root_))
    fs::create_directory(root_);
  LOG_IF(FATAL, !fs::is_directory(root_)) << "expected " << root_ << " to be a directory.";
}

CityStorage::~CityStorage() = default;

static re2::RE2 kCityDataFilePattern = RE2(".+\\.dat$");

static inline auto IsValidCityDataFile(const fs::directory_entry& dirent) -> bool {
  if (!dirent.is_regular_file())
    return false;
  const auto& filename = dirent.path().filename().string();
  return RE2::FullMatch(filename, kCityDataFilePattern);
}

auto CityStorage::LoadAllCities(std::vector<City>& results) -> bool {
  std::deque<fs::path> work{};
  for (const auto& dirent : fs::directory_iterator(GetRoot())) {
    if (!IsValidCityDataFile(dirent)) {
      DLOG(WARNING) << "skipping: " << dirent;
      continue;
    }
    work.push_back(dirent);
  }

  uint64_t idx = 0;
  results.resize(work.size());
  while (!work.empty()) {
    const auto next = work.back();
    work.pop_back();
    auto data_ptr = &results[idx++];
    if (!LoadCityFrom(next, data_ptr)) {
      LOG(ERROR) << "failed to load city from: " << next;
      return false;
    }
  }
  return true;
}

auto CityStorage::LoadCityFrom(const fs::path& p, City* result) -> bool {
  ASSERT(fs::exists(p) && fs::is_regular_file(p));
  DLOG(INFO) << "loading city from " << p << "....";
  std::fstream stream(p, std::ios::in | std::ios::binary);
  LOG_IF(FATAL, !stream.is_open()) << "failed to open city file: " << p;
  stream.seekg(0, std::ios::end);
  const auto filesize = stream.tellg();
  stream.seekg(0, std::ios::beg);
  std::vector<uint8_t> buffer{};
  buffer.resize(filesize);
  stream.read((char*)buffer.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  stream.close();
  (*result) = City(raw::GetCity(buffer.data()));
  return true;
}

void CityStorage::SaveCity(const City& rhs) const {
  const auto filename = GetRoot() / fmt::format("c{0:d}.dat", rhs.GetId());
  DVLOG(1) << "saving " << rhs.ToString() << " to " << filename << "....";
  LOG_IF(FATAL, !SaveCityTo(rhs, filename)) << "failed to save " << rhs << " to: " << filename;
}

void CityStorage::SaveAllCities(const std::vector<City>& cities) const {
  DLOG(INFO) << "saving all cities....";
  std::ranges::for_each(cities, [this](const City& c) {
    return SaveCity(c);
  });
}

auto CityStorage::SaveCityTo(const City& city, const fs::path& p) -> bool {
#ifdef PRT_DEBUG
  LOG_IF(WARNING, fs::exists(p)) << "overwriting " << p << "!";
#endif  // PRT_DEBUG
  flatbuffers::FlatBufferBuilder fbb{};
  city.SaveTo(fbb);
  std::fstream stream(p, std::ios::out | std::ios::binary | std::ios::trunc);
  LOG_IF(FATAL, !stream.is_open()) << "failed to open: " << p;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  stream.write((const char*)fbb.GetBufferPointer(), fbb.GetSize());
  stream.flush();
  stream.close();
  return true;
}
}  // namespace prt