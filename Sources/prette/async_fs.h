#ifndef PRT_ASYNC_FS_H
#define PRT_ASYNC_FS_H

#include <filesystem>
#include <functional>
#include <string>
#include <utility>
#include <uv.h>

#include "prette/common.h"
#include "prette/promise.h"
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
  FileHandle* file_;

 protected:
  explicit FsWorkTemplate(FileHandle* file) :
    WorkTemplate<uv_fs_t>(),
    file_(file) {
    handle()->data = this;
  }
  explicit FsWorkTemplate(const fs::path file) :
    FsWorkTemplate(new FileHandle(file)) {}

  virtual void DoWork() = 0;

 public:
  ~FsWorkTemplate() override = default;

  auto GetFile() const -> FileHandle* {
    return file_;
  }
};

using OpenFilePromise = Promise<FileHandle*>;
class OpenFileReq : public FsWorkTemplate {
 private:
  int flags_;
  int mode_;
  OpenFilePromise promise_;

  void DoWork() override;

 public:
  explicit OpenFileReq(const fs::path path, int flags, int mode, OpenFilePromise promise) :
    FsWorkTemplate(path),
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

using ReadFilePromise = Promise<const std::vector<uint8_t>&>;
class ReadFileReq : public FsWorkTemplate {
 private:
  std::vector<uint8_t> data_{};
  uv::Buffer buff_;
  ReadFilePromise promise_;

  void DoWork() override;

  auto Submit(uv_loop_t* loop) -> uv::Status {
    return uv_fs_read(loop, handle(), (uv::FileHandle)handle()->result, &buff_, 1, -1, OnWork);
  }

 public:
  explicit ReadFileReq(FileHandle* file, const uint64_t buffer_size, const ReadFilePromise promise) :
    FsWorkTemplate(file),
    promise_(std::move(promise)),
    data_(buffer_size),
    buff_(uv_buf_init((char*)&data_[0], data_.size())) {}  // NOLINT(cppcoreguidelines-pro-type-cstyle-cast)
  ~ReadFileReq() override = default;

  auto Submit(uv::Loop* loop) -> uv::Status override {
    ASSERT(loop);
    return uv_fs_read(*loop, handle(), GetFile()->file, &buff_, 1, -1, OnWork);
  }

  auto GetData() const -> const std::vector<uint8_t>& {
    return data_;
  }

  auto GetWorkName() const -> const char* override {
    return "ReadFile";
  }

  auto ToString() const -> std::string override;
};

auto OpenFile(fs::path path, int flags, int mode, OpenFilePromise::OnSuccessCallback on_next,
              OpenFilePromise::OnErrorCallback on_error = nullptr,
              OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq*;

static constexpr const auto kDefaultOpenMode = 0;
static inline auto OpenFile(fs::path path, int flags, OpenFilePromise::OnSuccessCallback on_next,
                            OpenFilePromise::OnErrorCallback on_error = nullptr,
                            OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq* {
  return OpenFile(path, flags, kDefaultOpenMode, on_next, on_error, on_complete);
}

static inline auto OpenFileReadOnly(fs::path path, int mode, OpenFilePromise::OnSuccessCallback on_next,
                                    OpenFilePromise::OnErrorCallback on_error = nullptr,
                                    OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq* {
  return OpenFile(path, O_RDONLY, mode, on_next, on_error, on_complete);
}

static inline auto OpenFileReadOnly(fs::path path, OpenFilePromise::OnSuccessCallback on_next,
                                    OpenFilePromise::OnErrorCallback on_error = nullptr,
                                    OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq* {
  return OpenFileReadOnly(path, kDefaultOpenMode, on_next, on_error, on_complete);
}

static inline auto OpenFileReadWrite(fs::path path, int mode, OpenFilePromise::OnSuccessCallback on_next,
                                     OpenFilePromise::OnErrorCallback on_error = nullptr,
                                     OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq* {
  return OpenFile(path, O_RDWR, mode, on_next, on_error, on_complete);
}

static inline auto OpenFileReadWrite(fs::path path, OpenFilePromise::OnSuccessCallback on_next,
                                     OpenFilePromise::OnErrorCallback on_error = nullptr,
                                     OpenFilePromise::OnCompleteCallback on_complete = nullptr) -> OpenFileReq* {
  return OpenFileReadWrite(path, kDefaultOpenMode, on_next, on_error, on_complete);
}

auto ReadFile(FileHandle* file, ReadFilePromise::OnSuccessCallback on_next, const uint64_t buffer_size = 4066,
              ReadFilePromise::OnErrorCallback on_error = nullptr,
              ReadFilePromise::OnCompleteCallback on_complete = nullptr) -> ReadFileReq*;

auto CloseFile(FileHandle* file, CloseFilePromise::OnSuccessCallback on_next = nullptr,
               CloseFilePromise::OnErrorCallback on_error = nullptr,
               CloseFilePromise::OnCompleteCallback on_complete = nullptr) -> CloseFileReq*;
}  // namespace prt::async

#endif  // PRT_ASYNC_FS_H
