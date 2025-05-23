#ifndef PRT_JSON_H
#define PRT_JSON_H

// IWYU pragma: begin_exports
#include <rapidjson/encodings.h>
#include <rapidjson/error/en.h>
#include <rapidjson/error/error.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>
// IWYU pragma: end_exports

#include <iostream>
#include <string>
#include <utility>
#include <vector>

#include "prette/common.h"

namespace prt::json {
using namespace rapidjson;

#define FOR_EACH_META_HANDLER_STATE(V) \
  V(OpenDoc)                           \
  V(ClosedDoc)                         \
  V(ParsingName)                       \
  V(ParsingTags)                       \
  V(Error)

class MetaHandler : public BaseReaderHandler<UTF8<>, MetaHandler> {
  enum State {
#define DEFINE_STATE(Name) k##Name,
    FOR_EACH_META_HANDLER_STATE(DEFINE_STATE)
#undef DEFINE_STATE
  };

 private:
  State state_ = kOpenDoc;
  std::string name_{};
  std::vector<std::string> tags_{};

  inline void SetState(const State rhs) {
    state_ = rhs;
  }

  inline auto NoTransition() -> bool {
    return true;
  }

  inline auto TransitionTo(const State state) -> bool {
    SetState(state);
    return !IsError();
  }

  inline auto InvalidState() -> bool {
    LOG(ERROR) << "invalid json::MetaHandler state: ";
    return false;
  }

 public:
  MetaHandler() = default;
  ~MetaHandler() = default;

  auto GetName() const -> const std::string& {
    return name_;
  }

  auto GetTags() const -> const std::vector<std::string>& {
    return tags_;
  }

  auto GetState() const -> State {
    return state_;
  }

#define DEFINE_STATE_CHECK(Name)         \
  inline auto Is##Name() const->bool {   \
    return GetState() == State::k##Name; \
  }
  FOR_EACH_META_HANDLER_STATE(DEFINE_STATE_CHECK)
#undef DEFINE_STATE_CHECK

  auto Null() -> bool {
    return true;
  }

  auto Bool(bool b) -> bool {
    return true;
  }

  auto Int(int i) -> bool {
    return true;
  }

  auto Uint(unsigned u) -> bool {
    return true;
  }

  auto Int64(int64_t i) -> bool {
    return true;
  }

  auto Uint64(uint64_t u) -> bool {
    return true;
  }

  auto Double(double d) -> bool {
    return true;
  }

  auto String(const char* str, SizeType length, bool copy) -> bool {
    const auto value = std::string(str, length);
    switch (GetState()) {
      case kParsingName: {
        name_ = std::string(value);
        return TransitionTo(State::kOpenDoc);
      }
      case kParsingTags:
        tags_.push_back(value);
        return NoTransition();
      default:
        return InvalidState();
    }
  }

  auto StartObject() -> bool {
    switch (GetState()) {
      case kOpenDoc:
        return true;
      default:
        return InvalidState();
    }
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool {
    const auto key = std::string(str, length);
    if (EqualsIgnoreCase(key, "name"))
      return TransitionTo(State::kParsingName);
    else if (EqualsIgnoreCase(key, "tags"))
      return TransitionTo(State::kParsingTags);
    LOG(FATAL) << "invalid meta document key: " << key;
  }

  auto EndObject(SizeType memberCount) -> bool {
    switch (GetState()) {
      case kOpenDoc:
        return TransitionTo(State::kClosedDoc);
      default:
        return InvalidState();
    }
  }

  auto StartArray() -> bool {
    switch (GetState()) {
      case kParsingTags:
        return NoTransition();
      default:
        return InvalidState();
    }
  }

  auto EndArray(SizeType elementCount) -> bool {
    switch (GetState()) {
      case kParsingTags:
        return TransitionTo(State::kOpenDoc);
      default:
        return InvalidState();
    }
  }
};

class TestDataHandler : public BaseReaderHandler<UTF8<>, TestDataHandler> {
  enum State {
    kEmptyDocument = 0,
    kClosedDocument,
    kOpenDocument,
    kParsingFilename,
  };

 private:
  State state_ = kEmptyDocument;

 public:
  TestDataHandler() = default;
  ~TestDataHandler() = default;

  auto GetState() const -> State {
    return state_;
  }

  inline auto IsEmpty() const -> bool {
    return GetState() == kEmptyDocument;
  }

  inline auto IsOpen() const -> bool {
    return GetState() == kOpenDocument;
  }

  inline auto IsClosed() const -> bool {
    return GetState() == kClosedDocument;
  }

  auto Null() -> bool {
    return true;
  }

  auto Bool(bool b) -> bool {
    return true;
  }

  auto Int(int i) -> bool {
    return true;
  }

  auto Uint(unsigned u) -> bool {
    return true;
  }

  auto Int64(int64_t i) -> bool {
    return true;
  }

  auto Uint64(uint64_t u) -> bool {
    return true;
  }

  auto Double(double d) -> bool {
    return true;
  }

  auto String(const char* str, SizeType length, bool copy) -> bool {
    const auto value = std::string(str, length);
    switch (GetState()) {
      case kParsingFilename: {
        DLOG(INFO) << "parsed data filename: " << value;
        state_ = kOpenDocument;
        break;
      }
      default:
        LOG(FATAL) << "invalid state.";
    }
    return true;
  }

  auto StartObject() -> bool {
    switch (GetState()) {
      case kEmptyDocument:
        state_ = kOpenDocument;
        break;
      default:
        LOG(FATAL) << "invalid data state";
    }
    return true;
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool {
    const auto key = std::string(str, length);
    if (EqualsIgnoreCase(key, "filename"))
      state_ = kParsingFilename;
    else
      LOG(FATAL) << "invalid data key: " << key;
    return true;
  }

  auto EndObject(SizeType memberCount) -> bool {
    LOG_IF(FATAL, !IsOpen()) << "cannot close unopened data document";
    state_ = kClosedDocument;
    return true;
  }

  auto StartArray() -> bool {
    return true;
  }

  auto EndArray(SizeType elementCount) -> bool {
    return true;
  }
};

template <class DataHandler>
class DocumentHandler : public BaseReaderHandler<UTF8<>, DocumentHandler<DataHandler>> {
  enum State {
    kEmptyDocument,
    kClosedDocument,
    kOpenDocument,
    kParsingType,
    kParsingMeta,
    kParsingData,
  };

 private:
  State state_ = kEmptyDocument;
  MetaHandler meta_{};
  DataHandler data_;

 public:
  template <typename... Args>
  DocumentHandler(Args... args) :
    data_(args...) {}
  ~DocumentHandler() = default;

  auto meta() const -> const MetaHandler& {
    return meta_;
  }

  auto GetState() const -> State {
    return state_;
  }

  auto HasError() const -> bool {
    return data_.HasError();
  }

  auto GetError() const -> std::string {
    if (data_.HasError())
      return data_.GetError();
    return "";
  }

  auto Null() -> bool {
    return true;
  }

  auto Bool(bool b) -> bool {
    return true;
  }

  auto Int(int i) -> bool {
    return true;
  }

  auto Uint(unsigned u) -> bool {
    return true;
  }

  auto Int64(int64_t i) -> bool {
    return true;
  }

  auto Uint64(uint64_t u) -> bool {
    return true;
  }

  auto Double(double d) -> bool {
    return true;
  }

  auto String(const char* str, SizeType length, bool copy) -> bool {
    const auto value = std::string(str, length);
    switch (GetState()) {
      case kParsingType: {
        state_ = kOpenDocument;
        return true;
      }
      case kParsingMeta:
        return meta_.String(str, length, copy);
      case kParsingData:
        return data_.String(str, length, copy);
      default:
        break;
    }
    LOG(FATAL) << "invalid state.";
  }

  auto StartObject() -> bool {
    switch (GetState()) {
      case kEmptyDocument:
        state_ = kOpenDocument;
        break;
      case kParsingMeta: {
        ASSERT(!meta_.IsClosedDoc());
        return meta_.StartObject();
      }
      case kParsingData: {
        return data_.StartObject();
      }
      default:
        LOG(FATAL) << "invalid state.";
    }
    return true;
  }

  auto Key(const char* str, SizeType length, bool copy) -> bool {
    const auto key = std::string(str, length);
    switch (GetState()) {
      case kOpenDocument: {
        if (EqualsIgnoreCase(key, "type"))
          state_ = kParsingType;
        else if (EqualsIgnoreCase(key, "meta"))
          state_ = kParsingMeta;
        else if (EqualsIgnoreCase(key, "data"))
          state_ = kParsingData;
        else
          LOG(FATAL) << "invalid document key: " << key;
        break;
      }
      case kParsingMeta:
        return meta_.Key(str, length, copy);
      case kParsingData:
        return data_.Key(str, length, copy);
      default:
        LOG(FATAL) << "invalid state.";
    }
    return true;
  }

  auto EndObject(SizeType memberCount) -> bool {
    switch (state_) {
      case kParsingMeta: {
        if (!meta_.EndObject(memberCount))
          return false;
        if (meta_.IsClosedDoc())
          state_ = kOpenDocument;
        break;
      }
      case kParsingData: {
        if (!data_.EndObject(memberCount))
          return false;
        if (data_.IsClosed())
          state_ = kOpenDocument;
        break;
      }
      case kOpenDocument:
        state_ = kClosedDocument;
        break;
      default:
        LOG(FATAL) << "invalid state.";
    }
    return true;
  }

  auto StartArray() -> bool {
    switch (GetState()) {
      case kParsingData:
        return data_.StartArray();
      default:
        break;
    }
    return true;
  }

  auto EndArray(SizeType elementCount) -> bool {
    switch (GetState()) {
      case kParsingData:
        return data_.EndArray(elementCount);
      default:
        break;
    }
    return true;
  }
};

template <typename State, class H>
class BaseStatefulReaderHandler : BaseReaderHandler<UTF8<>, H> {
 private:
  State state_;
  std::string error_;

 protected:
  explicit BaseStatefulReaderHandler(const State init_state) :
    state_(init_state) {}

  inline void SetState(const State rhs) {
    state_ = rhs;
  }

  inline void SetError(const std::string rhs) {
    ASSERT(!rhs.empty());
    error_ = std::move(rhs);
  }

  inline auto Error(const std::string rhs) -> bool {
    SetState(State::kError);
    SetError(std::move(rhs));
    return false;
  }

  inline auto InvalidState() -> bool {
    return Error("Invalid State");
  }

  inline auto Continue() const -> bool {
    return true;
  }

  inline auto TransitionTo(const State rhs) -> bool {
    SetState(rhs);
    return Continue();
  }

 public:
  virtual ~BaseStatefulReaderHandler() = default;

  auto GetState() const -> State {
    return state_;
  }

  auto GetError() const -> const std::string& {
    return error_;
  }

  inline auto HasError() const -> bool {
    return !error_.empty();
  }

  virtual auto Null() -> bool = 0;
  virtual auto Bool(bool b) -> bool = 0;
  virtual auto Int(int i) -> bool = 0;
  virtual auto Uint(unsigned u) -> bool = 0;
  virtual auto Int64(int64_t i) -> bool = 0;
  virtual auto Uint64(uint64_t u) -> bool = 0;
  virtual auto Double(double d) -> bool = 0;
  virtual auto Key(const char* str, SizeType length, bool copy) -> bool = 0;
  virtual auto String(const char* str, SizeType length, bool copy) -> bool = 0;
  virtual auto StartObject() -> bool = 0;
  virtual auto EndObject(SizeType memberCount) -> bool = 0;
  virtual auto StartArray() -> bool = 0;
  virtual auto EndArray(SizeType elementCount) -> bool = 0;
};

auto ReadJsonFromFile(fs::path path, std::string& result) -> bool;

template <typename H>
void ParseJsonDocumentFrom(fs::path path, H& handler) {
  std::string buffer{};
  LOG_IF(FATAL, !ReadJsonFromFile(path, buffer)) << "failed to read json from file: " << path;
  json::Reader reader{};
  json::StringStream ss(buffer.data());
  if (!reader.Parse(ss, handler)) {
    json::ParseErrorCode e = reader.GetParseErrorCode();
    size_t o = reader.GetErrorOffset();
    std::cerr << "Error: ";
    if (handler.HasError()) {
      std::cerr << handler.GetError();
    } else {
      std::cerr << json::GetParseError_En(e);
    }
    std::cerr << std::endl;
    std::cerr << " at offset " << o << " near '" << std::string(buffer.data()).substr(o, 10) << "...'" << std::endl;
  }
}
}  // namespace prt::json

#endif  // PRT_JSON_H
