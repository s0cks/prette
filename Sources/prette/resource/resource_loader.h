#ifndef PRT_RESOURCE_H
#error "Please #include <prette/resource/resource.h> instead."
#endif //PRT_RESOURCE_H

#ifndef PRT_RESOURCE_LOADER_H
#define PRT_RESOURCE_LOADER_H

#include <optional>
#include "prette/json.h"
#include "prette/reference.h"

namespace prt {
  namespace resource {
    template<typename R>
    class Loader {
    protected:
      Loader() = default;
    public:
      virtual ~Loader() = default;
      virtual Reference<R> Load() = 0;
    };

    template<typename R>
    class FileLoader : public Loader<R> {
    protected:
      std::string filename_;
      FILE* file_;

      explicit FileLoader(const std::string& filename):
        Loader<R>(),
        filename_(filename),
        file_(NULL) {
        if((file_ = fopen(filename_.c_str(), "rb")) == NULL)
          DLOG(ERROR) << "failed to open " << filename_ << ": " << ferror(file_);
      }

      inline void Close() {
        if(!IsOpen())
          return;
        fclose(file_);
        file_ = NULL;
      }
    public:
      ~FileLoader() override {
        Close();
      }

      bool IsOpen() const {
        return file_ != NULL;
      }
    };

    template<typename R>
    class JsonLoader : public Loader<R> {
    protected:
      json::Document& doc_;

      explicit JsonLoader(json::Document& doc):
        Loader<R>(),
        doc_(doc) {
      }

      inline std::optional<std::string> GetDocumentString(const char* name) const {
        if(!doc_.HasMember(name))
          return std::nullopt;
        const auto& value = doc_[name];
        if(!value.IsString()) {
          return std::nullopt;
        }
        return std::optional<std::string>{
          std::string(value.GetString(), value.GetStringLength()),
        };
      }

      inline std::optional<std::string> GetDocumentType() const {
        return GetDocumentString("type");
      }
    public:
      ~JsonLoader() override = default;
    };

    template<typename R>
    class JsonFileLoader : public FileLoader<R> {
    protected:
      json::Document doc_;
    
      explicit JsonFileLoader(const std::string& filename):
        FileLoader<R>(filename) {
        if(!json::ParseJson(FileLoader<R>::file_, doc_)) {
          DLOG(ERROR) << "failed to parse json document from: " << FileLoader<R>::filename_;
          FileLoader<R>::Close();
        }
      }

      inline std::optional<std::string> GetDocumentString(const char* name) const {
        if(!doc_.HasMember(name))
          return std::nullopt;
        const auto& value = doc_[name];
        if(!value.IsString()) {
          return std::nullopt;
        }
        return std::optional<std::string>{
          std::string(value.GetString(), value.GetStringLength()),
        };
      }

      inline std::optional<std::string> GetDocumentType() const {
        return GetDocumentString("type");
      }
    public:
      ~JsonFileLoader() override = default;
    };
  }
}

#endif //PRT_RESOURCE_LOADER_H