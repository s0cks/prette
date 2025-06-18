#include "prette/city/city_storage.h"

#include <filesystem>
#include <flatbuffers/buffer.h>
#include <flatbuffers/flatbuffer_builder.h>
#include <fmt/format.h>
#include <fstream>
#include <functional>
#include <ios>
#include <memory>
#include <re2/re2.h>
#include <rocksdb/iterator.h>
#include <rocksdb/options.h>
#include <string>
#include <utility>
#include <vector>

#include "prette/assertions.h"
#include "prette/city/city.h"
#include "prette/city/population.h"
#include "prette/common.h"
#include "prette/population_generated.h"

namespace prt {
CityStorage::CityStorage(const fs::path root) :
  root_(std::move(root)) {
  if (!fs::exists(root_))
    fs::create_directory(root_);
  LOG_IF(FATAL, !fs::is_directory(root_)) << "expected " << root_ << " to be a directory.";
  LoadIndex();
}

CityStorage::~CityStorage() {
  delete index_;
}

void CityStorage::LoadIndex() {
  const auto index_path = GetIndexPath();
  const bool is_genesis = !fs::exists(index_path);
  rocksdb::Options options{};
  options.create_if_missing = true;
  DB* db = nullptr;
  const auto status = DB::Open(options, index_path, &db);
  LOG_IF(FATAL, !status.ok()) << "failed to open settings database: " << status.ToString();
  ASSERT(db);
  index_ = db;
  ASSERT(index_);
}

static inline void WriteTo(const flatbuffers::FlatBufferBuilder& fbb, const fs::path& p) {
#ifdef PRT_DEBUG
  LOG_IF(WARNING, fs::exists(p)) << "overwriting " << p << "!";
#endif  // PRT_DEBUG
  std::fstream stream(p, std::ios::out | std::ios::binary | std::ios::trunc);
  LOG_IF(FATAL, !stream.is_open()) << "failed to open: " << p;
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  stream.write((const char*)fbb.GetBufferPointer(), fbb.GetSize());
  stream.flush();
  stream.close();
}

auto CityStorage::Save(City& rhs) -> bool {
  const auto& key = rhs.GetName();

  flatbuffers::FlatBufferBuilder fbb{};
  rhs.SaveTo(fbb);
  // NOLINTNEXTLINE(cppcoreguidelines-pro-type-cstyle-cast)
  std::string value((const char*)fbb.GetBufferPointer(), fbb.GetSize());

  rocksdb::WriteOptions options{};
  options.sync = true;
  const auto status = index()->Put(options, key, value);
  if (!status.ok()) {
    LOG(ERROR) << "failed to save " << rhs << " to index: " << status.ToString();
    return false;
  }
  SavePopulationTo(GetPopulationDataPath(rhs), rhs.GetPopulation());
  DLOG(INFO) << rhs << " saved!";
  return true;
}

void CityStorage::SavePopulationTo(const fs::path& p, Population& population) {
  std::fstream stream(p, std::ios::out | std::ios::binary | std::ios::trunc);
  flatbuffers::FlatBufferBuilder fbb{};
  const auto pop_offset = population.WriteTo(fbb);
  fbb.Finish(pop_offset);
  WriteTo(fbb, p);
}

void CityStorage::LoadPopulationFrom(const fs::path& p, Population& population) {
  ASSERT(fs::exists(p) && fs::is_regular_file(p));
  DLOG(INFO) << "loading population from " << p << "....";
  std::fstream stream(p, std::ios::in | std::ios::binary);
  LOG_IF(FATAL, !stream.is_open()) << "failed to open city file: " << p;
  stream.seekg(0, std::ios::end);
  const auto filesize = stream.tellg();
  stream.seekg(0, std::ios::beg);
  std::vector<uint8_t> buffer{};
  buffer.resize(filesize);
  stream.read((char*)buffer.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  stream.close();
  population.LoadFrom(*raw::GetPopulation(buffer.data()));
}

auto CityStorage::Load(const std::string& name) -> std::unique_ptr<City> {
  ASSERT_NOT_EMPTY(name);
  rocksdb::ReadOptions options{};
  std::string value{};
  const auto status = index()->Get(options, name, &value);
  if (status.IsNotFound()) {
    LOG(WARNING) << "failed to find City named `" << name << "` from index: " << status.ToString();
    return nullptr;
  }
  LOG_IF(FATAL, !status.ok()) << "failed to load City named `" << name << "` from index: " << status.ToString();
  const auto city = new City(raw::GetCity(value.data()));
  const auto pop_data = GetPopulationDataPath(*city);
  if (fs::exists(pop_data) && fs::is_regular_file(pop_data))
    LoadPopulationFrom(pop_data, city->GetPopulation());
  return std::unique_ptr<City>(city);
}

auto CityStorage::VisitAllCities(std::function<bool(const City&)> vis) -> bool {
  rocksdb::ReadOptions options{};
  const auto iter = index()->NewIterator(options);
  for (; iter->Valid(); iter->Next()) {
    std::string name(iter->key().data(), iter->key().size());
    City city(iter->value());
    DLOG(INFO) << "visiting `" << name << "`: " << city;
    if (!vis(city))
      return false;
  }
  return true;
}

// auto CityStorage::LoadAllCities(std::vector<City>& results, std::vector<std::vector<Citizen>>& citizens) -> bool {
//   std::deque<fs::path> work{};
//   for (const auto& dirent : fs::directory_iterator(GetRoot())) {
//     if (!IsValidCityDataFile(dirent)) {
//       DLOG(WARNING) << "skipping: " << dirent;
//       continue;
//     }
//     work.push_back(dirent);
//   }

//   uint64_t idx = 0;
//   results.resize(work.size());
//   citizens.resize(work.size());
//   while (!work.empty()) {
//     const auto next = work.back();
//     work.pop_back();
//     auto data_ptr = &results[idx];
//     if (!LoadCityFrom(next, data_ptr)) {
//       LOG(ERROR) << "failed to load city from: " << next;
//       return false;
//     }
//     const auto citizens_path = GetCitizensDataPath((*data_ptr));
//     LOG_IF(FATAL, !LoadCitizensFrom(citizens_path, citizens[idx])) << "failed to load citizens from: " <<
//     citizens_path; idx++;
//   }
//   return true;
// }

// auto CityStorage::LoadCitizensFrom(const fs::path& p, std::vector<Citizen>& results) -> bool {
//   ASSERT(fs::exists(p) && fs::is_regular_file(p));
//   DLOG(INFO) << "loading city from " << p << "....";
//   std::fstream stream(p, std::ios::in | std::ios::binary);
//   LOG_IF(FATAL, !stream.is_open()) << "failed to open city file: " << p;
//   stream.seekg(0, std::ios::end);
//   const auto filesize = stream.tellg();
//   stream.seekg(0, std::ios::beg);
//   std::vector<uint8_t> buffer{};
//   buffer.resize(filesize);
//   stream.read((char*)buffer.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
//   stream.close();
//   const auto citizens = raw::GetCitizenList(buffer.data());
//   results.reserve(citizens->data()->size());
//   for (const auto cz : *(citizens->data()))
//     results.emplace_back(*cz);
//   return true;
// }

// auto CityStorage::LoadCityFrom(const fs::path& p, City* result) -> bool {
//   ASSERT(fs::exists(p) && fs::is_regular_file(p));
//   DLOG(INFO) << "loading city from " << p << "....";
//   std::fstream stream(p, std::ios::in | std::ios::binary);
//   LOG_IF(FATAL, !stream.is_open()) << "failed to open city file: " << p;
//   stream.seekg(0, std::ios::end);
//   const auto filesize = stream.tellg();
//   stream.seekg(0, std::ios::beg);
//   std::vector<uint8_t> buffer{};
//   buffer.resize(filesize);
//   stream.read((char*)buffer.data(), filesize);  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
//   stream.close();
//   (*result) = City(raw::GetCity(buffer.data()));
//   return true;
// }

// void CityStorage::SaveCity(const City& rhs) const {
//   {
//     const auto filename = GetCityDataPath(rhs);
//     DVLOG(1) << "saving " << rhs.ToString() << " to " << filename << "....";
//     LOG_IF(FATAL, !SaveCityTo(rhs, filename)) << "failed to save " << rhs << " to: " << filename;
//   }
//   {
//     const auto filename = GetCitizensDataPath(rhs);
//     DVLOG(1) << "saving " << rhs.ToString() << " citizens to " << filename << "....";
//     LOG_IF(FATAL, !SaveCitizensTo(rhs, filename))
//         << "failed to save " << rhs.ToString() << " citizens to: " << filename;
//   }
// }

// void CityStorage::SaveAllCities(const std::vector<City>& cities) const {
//   DLOG(INFO) << "saving all cities....";
//   std::ranges::for_each(cities, [this](const City& c) {
//     return SaveCity(c);
//   });
// }

// auto CityStorage::SaveCityTo(const City& city, const fs::path& p) -> bool {
//   flatbuffers::FlatBufferBuilder fbb{};
//   city.SaveTo(fbb);
//   WriteTo(fbb, p);
//   return true;
// }

// auto CityStorage::SaveCitizensTo(const City& city, const fs::path& p) -> bool {
//   const auto city_manager = CityManager::Get();
//   // citizen data
//   flatbuffers::FlatBufferBuilder fbb{};
//   std::vector<flatbuffers::Offset<raw::Citizen>> data{};
//   data.reserve(city_manager->GetNumberOfCitizens(city));
//   city_manager->VisitAllCitizensForCity(city, [&data, &fbb](Citizen* p) {
//     data.push_back(p->WriteTo(fbb));
//     return true;
//   });
//   const auto data_offset = fbb.CreateVector(data);
//   raw::CitizenListBuilder builder(fbb);
//   builder.add_data(data_offset);
//   const auto citizens_offset = builder.Finish();
//   fbb.Finish(citizens_offset);
//   WriteTo(fbb, p);
//   return true;
// }
}  // namespace prt