#ifndef PRT_ASYNC_FS_H
#define PRT_ASYNC_FS_H

#include <filesystem>
#include <functional>
#include <string>
#include <sys/fcntl.h>
#include <utility>
#include <uv.h>

#include "prette/assertions.h"
#include "prette/common.h"
#include "prette/promise.h"
#include "prette/uv/handle.h"
#include "prette/uv/loop.h"
#include "prette/uv/status.h"
#include "prette/uv/utils.h"

namespace prt::async {
struct FileHandle {
  fs::path path;
  uv::FileHandle file = -1;

  FileHandle(const fs::path p) :
    path(std::move(p)) {}

  auto IsOpen() const -> bool {
    return file != -1;
  }
};

class FsWorkTemplate : public uv::WorkTemplate<uv_fs_t> {
 protected:
  template <typename T>
  static inline auto Unwrap(uv_fs_t* handle) -> T* {
    return uv::GetHandleData<uv_fs_t, T>(handle);
  }

  static inline void OnWork(uv_fs_t* handle) {
    return Unwrap<FsWorkTemplate>(handle)->DoWork();
  }

 private:
  void* data_;
  FileHandle* file_;

 protected:
  explicit FsWorkTemplate(FileHandle* file, void* data = nullptr) :
    WorkTemplate<uv_fs_t>(),
    file_(file),
    data_(data) {
    handle_ptr()->data = this;
  }
  explicit FsWorkTemplate(const fs::path file, void* data = nullptr) :
    FsWorkTemplate(new FileHandle(file), data) {}

  virtual void DoWork() = 0;

 public:
  ~FsWorkTemplate() override = default;

  auto GetData() const -> void* {
    return data_;
  }

  inline auto HasData() const -> bool {
    return GetData() != nullptr;
  }

  auto GetFile() const -> FileHandle* {
    return file_;
  }
};

using OpenFilePromise = Promise<void*, FileHandle*>;
class OpenFileReq : public FsWorkTemplate {
 private:
  int flags_;
  int mode_;
  OpenFilePromise promise_;

  void DoWork() override;

 public:
  explicit OpenFileReq(const fs::path path, int flags, int mode, OpenFilePromise promise, void* data = nullptr) :
    FsWorkTemplate(path, data),
    flags_(flags),
    mode_(mode),
    promise_(std::move(promise)) {}
  ~OpenFileReq() override = default;
  auto Submit(uv::Loop* loop) -> uv::Status override;
  auto GetWorkName() const -> const char* override {
    return "OpenFileReq";
  }

  auto ToString() const -> std::string override;
};

using CloseFilePromise = Promise<int>;
class CloseFileReq : public FsWorkTemplate {
 public:
  using OnSuccessCallback = std::function<void()>;
  using OnErrorCallback = std::function<void()>;
  using OnCompleteCallback = std::function<void()>;

 private:
  CloseFilePromise promise_;

  void DoWork() override;

 public:
  explicit CloseFileReq(FileHandle* file, CloseFilePromise promise) :
    FsWorkTemplate(file),
    promise_(std::move(promise)) {}
  ~CloseFileReq() override = default;
  auto Submit(uv::Loop* loop) -> uv::Status override;

  auto GetWorkName() const -> const char* override {
    return "CloseFile";
  }

  auto ToString() const -> std::string override;
};

using ReadFilePromise = Promise<void*, const std::vector<uint8_t>&>;
class ReadFileReq : public FsWorkTemplate {
 private:
  std::vector<uint8_t> bytes_{};
  uv::Buffer buff_;
  ReadFilePromise promise_;

  void DoWork() override;

  auto Submit(uv_loop_t* loop) -> uv::Status {
    return uv_fs_read(loop, handle_ptr(), (uv::FileHandle)handle_ptr()->result, &buff_, 1, -1, OnWork);
  }

 public:
  explicit ReadFileReq(FileHandle* file, const uint64_t buffer_size, const ReadFilePromise promise,
                       void* data = nullptr) :
    FsWorkTemplate(file, data),
    promise_(std::move(promise)),
    bytes_(buffer_size),
    buff_(uv_buf_init((char*)&bytes_[0], bytes_.size())) {}  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  ~ReadFileReq() override = default;

  auto Submit(uv::Loop* loop) -> uv::Status override {
    ASSERT(loop);
    return uv_fs_read(*loop, handle_ptr(), GetFile()->file, &buff_, 1, -1, OnWork);
  }

  auto GetBytes() const -> const std::vector<uint8_t>& {
    return bytes_;
  }

  auto GetWorkName() const -> const char* override {
    return "ReadFile";
  }

  auto ToString() const -> std::string override;
};

auto OpenFile(fs::path path, int flags, int mode, OpenFilePromise::OnSuccessCallback on_next,
              OpenFilePromise::OnErrorCallback on_error = nullptr,
              OpenFilePromise::OnCompleteCallback on_complete = nullptr, void* data = nullptr) -> OpenFileReq*;

static constexpr const auto kDefaultOpenMode = 0;
static inline auto OpenFile(fs::path path, int flags, OpenFilePromise::OnSuccessCallback on_next,
                            OpenFilePromise::OnErrorCallback on_error = nullptr,
                            OpenFilePromise::OnCompleteCallback on_complete = nullptr, void* data = nullptr)
    -> OpenFileReq* {
  return OpenFile(path, flags, kDefaultOpenMode, on_next, on_error, on_complete, data);
}

static inline auto OpenFileReadOnly(fs::path path, int mode, OpenFilePromise::OnSuccessCallback on_next,
                                    OpenFilePromise::OnErrorCallback on_error = nullptr,
                                    OpenFilePromise::OnCompleteCallback on_complete = nullptr, void* data = nullptr)
    -> OpenFileReq* {
  return OpenFile(path, O_RDONLY, mode, on_next, on_error, on_complete, data);
}

static inline auto OpenFileReadOnly(fs::path path, OpenFilePromise::OnSuccessCallback on_next,
                                    OpenFilePromise::OnErrorCallback on_error = nullptr,
                                    OpenFilePromise::OnCompleteCallback on_complete = nullptr, void* data = nullptr)
    -> OpenFileReq* {
  return OpenFileReadOnly(path, kDefaultOpenMode, on_next, on_error, on_complete, data);
}

static inline auto OpenFileReadWrite(fs::path path, int mode, OpenFilePromise::OnSuccessCallback on_next,
                                     OpenFilePromise::OnErrorCallback on_error = nullptr,
                                     OpenFilePromise::OnCompleteCallback on_complete = nullptr, void* data = nullptr)
    -> OpenFileReq* {
  return OpenFile(path, O_RDWR, mode, on_next, on_error, on_complete, data);
}

static inline auto OpenFileReadWrite(fs::path path, OpenFilePromise::OnSuccessCallback on_next,
                                     OpenFilePromise::OnErrorCallback on_error = nullptr,
                                     OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq* {
  return OpenFileReadWrite(path, kDefaultOpenMode, on_next, on_error, on_complete);
}

auto ReadFile(FileHandle* file, ReadFilePromise::OnSuccessCallback on_next, const uint64_t buffer_size = 4066,
              ReadFilePromise::OnErrorCallback on_error = nullptr,
              ReadFilePromise::OnCompleteCallback on_complete = nullptr, void* data = nullptr) -> ReadFileReq*;

auto CloseFile(FileHandle* file, CloseFilePromise::OnSuccessCallback on_next = nullptr,
               CloseFilePromise::OnErrorCallback on_error = nullptr,
               CloseFilePromise::OnCompleteCallback on_complete = nullptr) -> CloseFileReq*;
}  // namespace prt::async

#endif  // PRT_ASYNC_FS_H
