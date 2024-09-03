#include "prette/settings.h"
#include "prette/engine/engine.h"

#include "prette/thread_local.h"

namespace prt::settings {
  static inline std::ostream& 
  operator<<(std::ostream& stream, const leveldb::Status& status) {
    return stream << status.ToString();
  }

#define WRITE_OPTIONS                 \
  leveldb::WriteOptions write_opts;   \
  write_opts.sync = true;
#define READ_OPTIONS const auto read_opts = leveldb::ReadOptions();

  DEFINE_string(settings_dir, "", "The direction to store settings.");

  static inline std::string
  GetSettingsFilename() {
    return FLAGS_settings_dir + "/settings";
  }

  static leveldb::DB* index_;

  static inline leveldb::DB*
  GetIndex() {
    return index_;
  }

  template<const google::LogSeverity Severity>
  static inline void
  InitializeIndex(const std::string& filename) {
    leveldb::Options options;
    options.create_if_missing = true;

    leveldb::Status status;
    if(!(status = leveldb::DB::Open(options, filename, &index_)).ok())
      LOG_AT_LEVEL(Severity) << "failed to initialize settings repository in: " << filename;
  }

  void OnPreInit() {
    InitializeIndex<google::FATAL>(GetSettingsFilename());
  }

  void OnInit() {

  }

  void OnPostInit() {

  }

  void Init() {
    //TODO:
    // Engine::OnPreInit(&OnPreInit);
    // Engine::OnInit(&OnInit);
    // Engine::OnPostInit(&OnPostInit);
  }

#define FAILED_TO_GET(Name, Type) ({                                                          \
  LOG(ERROR) << "failed to get " << #Name << " (" << #Type << ") from settings: " << status;  \
  return std::nullopt;                                                                        \
})

#define FAILED_TO_PUT(Name, Type) ({                                                          \
  LOG(ERROR) << "failed to put " << #Name << " (" << #Type << ") in settings: " << status;    \
  return false;                                                                               \
})

  bool PutString(const std::string& name, const std::string& value) {
    WRITE_OPTIONS;
    leveldb::Status status;
    if(!(status = index_->Put(write_opts, (const leveldb::Slice&) name, (const leveldb::Slice&) value)).ok())
      FAILED_TO_PUT(name, std::string);
    return true;
  }

  std::optional<std::string> GetString(const std::string& name) {
    READ_OPTIONS;

    std::string result;
    leveldb::Status status;
    if(!(status = index_->Get(read_opts, (const leveldb::Slice&) name, &result)).ok())
      FAILED_TO_GET(name, std::string);
    return std::optional<std::string>(result);
  }

  bool PutVec2(const std::string& name, const glm::vec2& value) {
    leveldb::Slice slice((const char*) glm::value_ptr(value), (sizeof(float) * 2) + 1);
    WRITE_OPTIONS;
    leveldb::Status status;
    if(!(status = GetIndex()->Put(write_opts, (const leveldb::Slice&) name, slice)).ok())
      FAILED_TO_PUT(name, glm::vec2);
    return true;
  }

  std::optional<glm::vec2> GetVec2(const std::string& name) {
    READ_OPTIONS;
    std::string result;
    leveldb::Status status;
    if(!(status = GetIndex()->Get(read_opts, (const leveldb::Slice&) name, &result)).ok())
      FAILED_TO_GET(name, glm::vec2);
    const auto data = (const float*)result.data();
    return std::optional<glm::vec2>(glm::vec2(data[0], data[1]));
  }

  bool PutVec3(const std::string& name, const glm::vec3& value) {
    leveldb::Slice slice((const char*) glm::value_ptr(value), (sizeof(float) * 3) + 1);
    WRITE_OPTIONS;
    leveldb::Status status;
    if(!(status = GetIndex()->Put(write_opts, (const leveldb::Slice&) name, slice)).ok())
      FAILED_TO_PUT(name, glm::vec3);
    return true;
  }

  std::optional<glm::vec3> GetVec3(const std::string& name) {
    READ_OPTIONS;
    std::string result;
    leveldb::Status status;
    if(!(status = GetIndex()->Get(read_opts, (const leveldb::Slice&) name, &result)).ok())
      FAILED_TO_GET(name, glm::vec3);
    const auto data = (const float*)result.data();
    return std::optional<glm::vec3>(glm::vec3(data[0], data[1], data[2]));
  }
}