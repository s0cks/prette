#include <exception>
#include <fmt/format.h>
#include <stdexcept>
#include <utility>

#include "prette/assertions.h"
#include "prette/async_fs.h"
#include "prette/common.h"
#include "prette/scheduler.h"
#include "prette/to_string.h"
#include "prette/uv/utils.h"

namespace prt::async {
void OpenFileReq::DoWork() {
  if (handle()->result < 0) {
    try {
      const auto error = uv_strerror(static_cast<int>(handle()->result));
      const auto message = fmt::format("failed to open file `{0:s}`: {1:s}", GetFile()->path.c_str(), error);
      throw std::runtime_error(message);
    } catch (const std::exception& exc) {
      promise_.on_error(std::current_exception());
    }
  } else {
    GetFile()->file = static_cast<uv::FileHandle>(handle()->result);
    promise_.on_success(GetFile());
  }
  promise_.on_complete();
  uv_fs_req_cleanup(handle());
}

auto OpenFileReq::Submit(uv::Loop* loop) -> uv::Status {
  ASSERT(loop);
  return uv_fs_open(*loop, handle(), GetFile()->path.c_str(), flags_, mode_, FsWorkTemplate::OnWork);
}

auto OpenFileReq::ToString() const -> std::string {
  return ToStringHelper<OpenFileReq>{};
}

auto OpenFile(fs::path path, int flags, int mode, OpenFilePromise::OnSuccessCallback on_success,
              OpenFilePromise::OnErrorCallback on_error, OpenFilePromise::OnCompleteCallback on_complete)
    -> OpenFileReq* {
  auto& scheduler = GetEngineScheduler();
  OpenFilePromise promise(on_success, on_error, on_complete);
  return scheduler.Schedule<OpenFileReq>(path, flags, mode, std::move(promise));
}

void ReadFileReq::DoWork() {
  if (handle()->result == -1) {
    try {
      const auto error = uv_strerror(static_cast<int>(handle()->result));
      const auto message = fmt::format("failed to read file `{0:s}`: {1:s}", GetFile()->path.c_str(), error);
      throw std::runtime_error(message);
    } catch (const std::exception& exc) {
      promise_.on_error(std::current_exception());
    }
  } else if (handle()->result > 1) {
    promise_.on_success(data_);
    const auto status = Submit(handle()->loop);
    LOG_IF(FATAL, !status) << "failed to submit new read work: " << status;
    if (handle()->result >= data_.capacity())
      return;
  } else if (handle()->result == 1) {
    uv_fs_req_cleanup(handle());
    promise_.on_complete();
  }
}

auto ReadFileReq::ToString() const -> std::string {
  return ToStringHelper<ReadFileReq>{};
}

auto ReadFile(FileHandle* file, ReadFilePromise::OnSuccessCallback on_next, const uint64_t buffer_size,
              ReadFilePromise::OnErrorCallback on_error, ReadFilePromise::OnCompleteCallback on_complete)
    -> ReadFileReq* {
  ASSERT(file && file->IsOpen());
  auto& scheduler = GetEngineScheduler();
  ReadFilePromise promise(on_next, on_error, on_complete);
  return scheduler.Schedule<ReadFileReq>(file, buffer_size, std::move(promise));
}

auto CloseFileReq::ToString() const -> std::string {
  return ToStringHelper<CloseFileReq>{};
}

void CloseFileReq::DoWork() {
  uv_fs_req_cleanup(handle());
  if (handle()->result == -1) {
    try {
      const auto error = uv_strerror(static_cast<int>(handle()->result));
      const auto message = fmt::format("failed to read file `{0:s}`: {1:s}", GetFile()->path.c_str(), error);
      throw std::runtime_error(message);
    } catch (const std::exception& exc) {
      promise_.on_error(std::current_exception());
    }
  } else {
    promise_.on_success(static_cast<int>(handle()->result));
  }
  promise_.on_complete();
}

auto CloseFileReq::Submit(uv::Loop* loop) -> uv::Status {
  ASSERT(loop);
  return uv_fs_close(*loop, handle(), GetFile()->file, FsWorkTemplate::OnWork);
}

auto CloseFile(FileHandle* file, CloseFilePromise::OnSuccessCallback on_next,
               CloseFilePromise::OnErrorCallback on_error, CloseFilePromise::OnCompleteCallback on_complete)
    -> CloseFileReq* {
  ASSERT(file && file->IsOpen());
  auto& scheduler = GetEngineScheduler();
  CloseFilePromise promise(on_next, on_error, on_complete);
  return scheduler.Schedule<CloseFileReq>(file, promise);
}
}  // namespace prt::async