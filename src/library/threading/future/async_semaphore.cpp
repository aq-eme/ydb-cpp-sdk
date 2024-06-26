#include "async_semaphore.h"

#include <ydb-cpp-sdk/util/system/guard.h>
#include <ydb-cpp-sdk/util/system/yassert.h>

#include <src/library/threading/cancellation/operation_cancelled_exception.h>

#include <mutex>

namespace NThreading {

TAsyncSemaphore::TAsyncSemaphore(size_t count)
    : Count_(count)
{
    Y_ASSERT(count > 0);
}

TAsyncSemaphore::TPtr TAsyncSemaphore::Make(size_t count) {
    return TPtr(new TAsyncSemaphore(count));
}

TFuture<TAsyncSemaphore::TPtr> TAsyncSemaphore::AcquireAsync() {
    std::lock_guard guard(Lock_);
    if (Cancelled_) {
        return MakeErrorFuture<TPtr>(
            std::make_exception_ptr(TOperationCancelledException()));
    }
    if (Count_) {
        --Count_;
        return MakeFuture<TAsyncSemaphore::TPtr>(this);
    }
    auto promise = NewPromise<TAsyncSemaphore::TPtr>();
    Promises_.push_back(promise);
    return promise.GetFuture();
}

void TAsyncSemaphore::Release() {
    TPromise<TPtr> promise;
    {
        std::lock_guard guard(Lock_);
        if (Cancelled_) {
            return;
        }
        if (Promises_.empty()) {
            ++Count_;
            return;
        } else {
            promise = Promises_.front();
            Promises_.pop_front();
        }
    }
    promise.SetValue(this);
}

void TAsyncSemaphore::Cancel() {
    std::list<TPromise<TPtr>> promises;
    {
        std::lock_guard guard(Lock_);
        Cancelled_ = true;
        std::swap(Promises_, promises);
    }
    for (auto& p: promises) {
        p.SetException(std::make_exception_ptr(TOperationCancelledException()));
    }
}

TAsyncSemaphore::TAutoRelease::~TAutoRelease() {
    if (Sem) {
        Sem->Release();
    }
}

std::function<void (const TFuture<void>&)> TAsyncSemaphore::TAutoRelease::DeferRelease() {
    return [s = std::move(this->Sem)](const TFuture<void>&) {
        s->Release();
    };
}

}
