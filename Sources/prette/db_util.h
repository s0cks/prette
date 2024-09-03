#ifndef PRT_DB_UTIL_H
#define PRT_DB_UTIL_H

#include <glog/logging.h>
#include <leveldb/db.h>

namespace prt::db {
  static inline std::ostream&
  operator<<(std::ostream& stream, const leveldb::Status& rhs) {
    return stream << rhs.ToString();
  }

  static inline leveldb::Status
  OpenWith(const std::string& path, const leveldb::Options& options, leveldb::DB** result) {
    return leveldb::DB::Open(options, path, result);
  }

  template<const google::LogSeverity Severity = google::ERROR>
  static inline void
  TryOpenWith(const std::string& path, const leveldb::Options& options, leveldb::DB** result) {
    const auto status = OpenWith(path, options, result);
    if(!status.ok()) {
      LOG_AT_LEVEL(Severity) << "failed to open " << path << ": " << status;
      return;
    }
    DLOG(INFO) << "opened " << path << ".";
  }
}

#endif //PRT_DB_UTIL_H