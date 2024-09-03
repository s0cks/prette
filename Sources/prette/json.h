#ifndef PRT_JSON_H
#define PRT_JSON_H

#include <cstdio>
#include <string>
#include <memory>
#include <optional>
#include <glog/logging.h>
#include <rapidjson/reader.h>
#include <rapidjson/writer.h>
#include <rapidjson/schema.h>
#include <rapidjson/document.h>
#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/filereadstream.h>

#include "prette/rx.h"
#include "prette/uri.h"
#include "prette/buffer.h"
#include "prette/common.h"
#include "prette/metadata.h"
#include "prette/json_flags.h"

namespace prt::json {
  using namespace rapidjson;

  typedef Document::Object Object;
  typedef Document::ConstObject ConstObject;

  typedef std::shared_ptr<Document> DocumentPtr;

  bool ParseJson(FILE* file, Document& doc);
  bool ParseJson(const uri::Uri& uri, Document& doc);

  static inline bool
  ParseJson(const uri::basic_uri& uri, Document& doc) {
    return ParseJson(uri::Uri(uri), doc);
  }

  bool ParseRawJson(const char* json, Document& doc);

  static inline bool
  ParseRawJson(const std::string& json, Document& doc) {
    return ParseRawJson(json.data(), doc);
  }

  static constexpr const auto kDefaultJsonFileBufferSize = 4096;
  rx::observable<DocumentPtr> ParseDocument(FILE* file, const int buffer_size = kDefaultJsonFileBufferSize);

  static inline rx::observable<DocumentPtr>
  ParseDocument(const uri::Uri& uri, const int buffer_size = kDefaultJsonFileBufferSize) {
    auto file = fopen(uri.path.c_str(), "rb");
    if(!file)
      return rx::observable<>::error<DocumentPtr>(std::runtime_error(""));
    return ParseDocument(file, buffer_size);
  }

  static inline std::string
  GetParseError(const Document& doc) {
    PRT_ASSERT(doc.HasParseError());
    std::stringstream ss;
    switch(doc.GetParseError()) {
      case kParseErrorDocumentEmpty:
        ss << "Document is empty.";
        break;
      case kParseErrorDocumentRootNotSingular:
        ss << "Document has multiple root values.";
        break;
      case kParseErrorValueInvalid:
        ss << "Valid is invalid.";
        break;
      default:
        ss << "Unknown: " << static_cast<uint64_t>(doc.GetParseError());
        break;
    }
    return ss.str(); //TODO: optimize and improve this
  }

  static inline bool
  Validate(SchemaDocument& schema, Document& doc) {
    SchemaValidator validator(schema);
    if (!doc.Accept(validator)) {
      // Input JSON is invalid according to the schema
      // Output diagnostic information
      StringBuffer sb;
      validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
      printf("Invalid schema: %s\n", sb.GetString());
      printf("Invalid keyword: %s\n", validator.GetInvalidSchemaKeyword());
      sb.Clear();
      validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
      printf("Invalid document: %s\n", sb.GetString());
      return false;
    }
    return true;
  }

  static inline bool
  Validate(SchemaDocument& schema, const std::string& filename) {
    Document doc;
    if(!ParseJson(filename, doc))
      return false;
    return Validate(schema, doc);
  }

  static inline bool
  Validate(const std::string& schemaf, const std::string& filename) {
    Document schemad;
    if(!ParseJson(schemaf, schemad))
      return false;
    SchemaDocument schema(schemad);
    return Validate(schema, filename);
  }

  class Spec;
  class SpecSchema {
    friend class Spec;
    friend class SpecSchemaTest;
  protected:
    json::SchemaDocument doc_;
    
    explicit SpecSchema(const json::Document& doc):
      doc_(doc) {
    }
  public:
    virtual ~SpecSchema() = default;
    virtual Spec* GetSpec() const = 0;
    
    virtual bool IsValid(const json::Document& doc) const {
      json::SchemaValidator validator(doc_);
      if(doc.Accept(validator))
        return true;
      //TODO: print errors
      // StringBuffer sb;
      // validator.GetInvalidSchemaPointer().StringifyUriFragment(sb);
      // printf("Invalid schema: %s\n", sb.GetString());
      // printf("Invalid keyword: %s\n", validator.GetInvalidSchemaKeyword());
      // sb.Clear();
      // validator.GetInvalidDocumentPointer().StringifyUriFragment(sb);
      // printf("Invalid document: %s\n", sb.GetString());
      // return false;
      return false;
    }
  };

  class JsonSpec {
  protected:
    uri::Uri uri_;
    DocumentPtr doc_;

    JsonSpec(const uri::Uri& uri, DocumentPtr doc):
      uri_(uri),
      doc_(doc) {
      PRT_ASSERT(doc.get());
      PRT_ASSERT(uri.HasScheme("file"));
      PRT_ASSERT(uri.HasExtension(".json"));
    }

    inline DocumentPtr doc() const {
      return doc_;
    }

    inline std::optional<std::string>
    GetStringProperty(const char* name) const {
      if(!doc_->HasMember(name))
        return std::nullopt;
      const auto& property = (*doc_)[name];
      if(!property.IsString())
        return std::nullopt;
      const std::string value(property.GetString(), property.GetStringLength());
      return { value };
    }

    inline std::optional<uri::Uri>
    GetUriProperty(const char* name) const {
      const auto property = GetStringProperty(name);
      return property ? std::optional<uri::Uri>(uri::Uri(*property)) : std::nullopt;
    }
  public:
    virtual ~JsonSpec() = default;
  };

#define FOR_EACH_JSON_READER_TEMPLATE_STATE(V)  \
  V(Error)                                      \
  V(Closed)                                     \
  V(Open)                                       \
  V(Type)                                       \
  V(Meta)                                       \
  V(MetaTags)                                   \
  V(MetaName)                                   \
  V(Data)

  template<typename State, class D>
  class ReaderHandlerTemplate : public BaseReaderHandler<UTF8<>, D> {
  protected:
    std::string type_;
    State state_;
    Metadata meta_;

    explicit ReaderHandlerTemplate(const std::string& type,
                                   const State init_state = State::kClosed):
      BaseReaderHandler<UTF8<>, D>(),
      type_(type),
      state_(init_state) {
    }

    inline State GetState() const {
      return state_;
    }

    inline void SetState(const State state) {
      state_ = state;
    }

    inline bool TransitionTo(const State state) {
      if(state_ == state)
        return true;
      SetState(state);
      return state != State::kError;
    }

    inline bool NoTransition() {
      return GetState() != State::kError;
    }

    bool OnParseSpecField(const std::string& name) {
      if(EqualsIgnoreCase(name, "type")) {
        return TransitionTo(State::kType);
      } else if(EqualsIgnoreCase(name, "meta")) {
        return TransitionTo(State::kMeta);
      } else if(EqualsIgnoreCase(name, "data")) {
        return TransitionTo(State::kData);
      }
      return TransitionTo(State::kError);
    }

    bool OnParseMetaField(const std::string& name) {
      if(EqualsIgnoreCase(name, "name")) {
        return TransitionTo(State::kMetaName);
      } else if(EqualsIgnoreCase(name, "tags")) {
        return TransitionTo(State::kMetaTags);
      }
      return TransitionTo(State::kError);
    }

    virtual bool OnParseDataField(const std::string& name) = 0;
    
    virtual bool OnParseField(const std::string& name) {
      switch(GetState()) {
        case State::kOpen:
          return OnParseSpecField(name);
        case State::kMeta:
          return OnParseMetaField(name);
        case State::kData:
          return OnParseDataField(name);
        default: return TransitionTo(State::kError);
      }
    }

    virtual bool OnParseType(const std::string& type) {
      if(!EqualsIgnoreCase(type, type_))
        return TransitionTo(State::kError);
      return TransitionTo(State::kOpen);
    }
    
    virtual bool OnParseMetaName(const std::string& name) {
      meta_.SetName(name);
      return TransitionTo(State::kMeta);
    }

    virtual bool OnParseMetaTag(const std::string& value) {
      meta_.Append(Tag(value));
      return TransitionTo(State::kMetaTags);
    }
  public:
    virtual ~ReaderHandlerTemplate() = default;

    const Metadata& GetMeta() const {
      return meta_;
    }
    
    virtual bool StartArray() {
      switch(GetState()) {
        case State::kMetaTags:
          return TransitionTo(GetState());
        default:
          return TransitionTo(State::kError);
      }
    }

    virtual bool EndArray(const rapidjson::SizeType size) {
      switch(GetState()) {
        case State::kMetaTags:
          return TransitionTo(State::kMeta);
        default:
          return TransitionTo(State::kError);
      }
    }

    virtual bool StartObject() {
      switch(GetState()) {
        case State::kClosed:
          return TransitionTo(State::kOpen);
        case State::kMeta:
        case State::kData:
          return TransitionTo(GetState());
        default: return TransitionTo(State::kError);
      }
    }

    virtual bool EndObject(const rapidjson::SizeType size) {
      switch(GetState()) {
        case State::kOpen:
          return TransitionTo(State::kClosed);
        case State::kMeta:
        case State::kData:
          return TransitionTo(State::kOpen);
        default: return TransitionTo(State::kError);
      }
    }

    virtual bool String(const char* value, const rapidjson::SizeType length, const bool) {
      switch(GetState()) {
        case State::kOpen:
        case State::kMeta:
        case State::kData:
          return OnParseField(std::string(value, length));
        case State::kType:
          return OnParseType(std::string(value, length));
        case State::kMetaName:
          return OnParseMetaName(std::string(value, length));
        case State::kMetaTags:
          return OnParseMetaTag(std::string(value, length));
        default:
          return TransitionTo(State::kError);
      }
    }

    virtual bool Default() {
      return TransitionTo(State::kError);
    }
  };

  class JsonParseResult {
    DEFINE_DEFAULT_COPYABLE_TYPE(JsonParseResult);
  protected:
    bool success_;
    std::string message_;
  public:
    JsonParseResult() = default;
    explicit JsonParseResult(const bool success, const std::string& message = {}):
      success_(success),
      message_(message) {
    }
    ~JsonParseResult() = default;

    std::string message() const {
      return message_;
    }

    bool success() const {
      return success_;
    }

    operator bool() const {
      return success_;
    }

    explicit operator std::string() const {
      return message_;
    }

    bool operator==(const JsonParseResult& rhs) const {
      return success_ == rhs.success_
          && message_ == rhs.message_;
    }

    bool operator!=(const JsonParseResult& rhs) const {
      return success_ != rhs.success_
          || message_ != rhs.message_;
    }

    friend std::ostream& operator<<(std::ostream& stream, const JsonParseResult& rhs) {
      if(rhs.success())
        return stream << "Success.";
      stream << "Parse Failed: " << rhs.message();
      return stream;
    }
  };

  template<class H>
  static inline JsonParseResult
  ParseJson(const uri::Uri& uri, H& handler, const uword buffer_size = kDefaultJsonFileBufferSize) {
    auto file = uri.OpenFileForReading();
    if(!file)
      return JsonParseResult(false, fmt::format("failed to open json file: {0:s}", uri));
    auto buffer = Buffer::New(buffer_size);
    json::FileReadStream frs(file, (char*) buffer->data(), buffer_size);
    json::Reader reader;
    if(!reader.Parse(frs, handler)) {
      const auto code = reader.GetParseErrorCode();
      const auto offset = reader.GetErrorOffset();
      const auto error = fmt::format("failed to parse json from file {0:s}: {1:s} at {2:d}", uri, json::GetParseError_En(code), offset);
      fclose(file);
      return JsonParseResult(false, error);
    }
    fclose(file);
    return JsonParseResult(true);
  }
}

#endif //PRT_JSON_H