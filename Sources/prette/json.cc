#include "prette/json.h"
#include "prette/buffer.h"

namespace prt::json {
  static inline DocumentPtr
  NewDocument() {
    return std::make_shared<Document>();
  }

  rx::observable<DocumentPtr> ParseDocument(FILE* file, const int buffer_size) {
    return rx::observable<>::create<DocumentPtr>([&file,buffer_size](rx::subscriber<DocumentPtr> s) {
      const auto buffer = Buffer::New(buffer_size);
      const auto doc = NewDocument();
      FileReadStream frs(file, (char*) buffer->data(), 4096);
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

  bool ParseJson(FILE* file, Document& doc) {
    auto buffer = Buffer::New(4096);
    FileReadStream frs(file, (char*) buffer->data(), 4096);
    doc.ParseStream(frs);
    if(doc.HasParseError()) {
      fclose(file);
      return false;
    }
    
    fclose(file);
    return true;
  }

  bool ParseJson(const uri::Uri& uri, Document& doc) {
    DLOG(INFO) << "parsing json::Document from: " << uri;
    auto file = uri.OpenFileForReading();
    if(!file) {
      DLOG(ERROR) << "failed to open json file: " << uri;
      return false;
    }
    return ParseJson(file, doc);
  }

  bool ParseRawJson(const char* value, Document& doc) {
    StringStream ss(value);
    doc.ParseStream(ss);
    return !doc.HasParseError();
  }
}