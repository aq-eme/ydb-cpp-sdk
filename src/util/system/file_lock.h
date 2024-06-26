#pragma once

#include <ydb-cpp-sdk/util/generic/fwd.h>
#include <ydb-cpp-sdk/util/generic/noncopyable.h>
#include <ydb-cpp-sdk/util/system/file.h>

enum class EFileLockType {
    Exclusive,
    Shared
};

class TFileLock: public TFile {
public:
    TFileLock(const std::string& filename, const EFileLockType type = EFileLockType::Exclusive);

    void Acquire();
    bool TryAcquire();
    void Release();

    inline void lock() {
        Acquire();
    }

    inline bool try_lock() {
        return TryAcquire();
    }

    inline void unlock() {
        Release();
    }

private:
    EFileLockType Type;
};
