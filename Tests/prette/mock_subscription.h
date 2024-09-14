#ifndef PRT_MOCK_SUBSCRIPTION_H
#define PRT_MOCK_SUBSCRIPTION_H

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <glog/logging.h>

#include "prette/rx.h"

namespace prt::rx {
  template<typename T>
  class MockSubscription {
  public:
    explicit MockSubscription() {
      ON_CALL((*this), OnNext)
        .WillByDefault([this](const T& next) {
          DLOG(INFO) << "next: " << next;
        });
      ON_CALL((*this), OnError)
        .WillByDefault([this](rx::error_ptr err) {
          DLOG(ERROR) << "error: " << rx::what(err).c_str();
        });
      ON_CALL((*this), OnCompleted)
        .WillByDefault([this]() {
          DLOG(INFO) << "completed.";
        });
    }
    ~MockSubscription() = default;

    MOCK_METHOD1(OnNext, void(const T&)); //NOLINT(modernize-use-trailing-return-type)
    MOCK_METHOD1(OnError, void(rx::error_ptr)); //NOLINT(modernize-use-trailing-return-type)
    MOCK_METHOD0(OnCompleted, void()); //NOLINT(modernize-use-trailing-return-type)

    auto Subscribe(rx::observable<T> observable) -> rx::composite_subscription {
      const auto onnext = [this](const T& next) { return OnNext(next); };
      const auto onerror = [this](const rx::error_ptr& err) { return OnError(err); };
      const auto oncompleted = [this]() { return OnCompleted(); };
      return observable.subscribe(onnext, onerror, oncompleted);
    }
  };
}

#endif //PRT_MOCK_SUBSCRIPTION_H