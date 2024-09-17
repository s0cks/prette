#include "prette/json.h"
#include "prette/buffer.h"

namespace prt::json {
  static inline auto
  NewDocument() -> DocumentPtr {
    return std::make_shared<Document>();
  }

  auto ParseDocument(FILE* file, const int buffer_size) -> rx::observable<DocumentPtr> {
    return rx::observable<>::create<DocumentPtr>([&file,buffer_size](rx::subscriber<DocumentPtr> s) {
      static constexpr const auto kDefaultJsonDocumentBufferSize = 4096;
      const auto buffer = Buffer::New(buffer_size);
      const auto doc = NewDocument();
      FileReadStream frs(file, (char*) buffer->data(), kDefaultJsonDocumentBufferSize); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
      doc->ParseStream(frs);
      if(doc->HasParseError()) {
        fclose(file);
        s.on_error(std::make_exception_ptr(std::runtime_error("")));
        return;
      }
      s.on_next(doc);
      s.on_completed();
    });
  }

  auto ParseJson(FILE* file, Document& doc) -> bool {
    static constexpr const auto kDefaultJsonBufferSize = 4096;
    auto buffer = Buffer::New(kDefaultJsonBufferSize);
    FileReadStream frs(file, (char*) buffer->data(), buffer->capacity()); // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
    doc.ParseStream(frs);
    if(doc.HasParseError()) {
      fclose(file);
      return false;
    }

    fclose(file);
    return true;
  }

  auto ParseJson(const uri::Uri& uri, Document& doc) -> bool {
    DLOG(INFO) << "parsing json::Document from: " << uri;
    auto file = uri.OpenFileForReading();
    if(!file) {
      DLOG(ERROR) << "failed to open json file: " << uri;
      return false;
    }
    return ParseJson(file, doc);
  }

  auto ParseRawJson(const char* value, Document& doc) -> bool {
    StringStream ss(value);
    doc.ParseStream(ss);
    return !doc.HasParseError();
  }
}