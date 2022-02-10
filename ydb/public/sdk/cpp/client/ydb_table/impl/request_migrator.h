#pragma once

#include "client_session.h" 

#include <ydb/public/sdk/cpp/client/ydb_table/table.h>
 
#include <library/cpp/threading/future/future.h>

#include <util/generic/string.h>

#include <mutex>
#include <memory>

namespace NYdb {

namespace NMath {

struct TStats {
    const ui64 Cv;
    const float Mean;
};

TStats CalcCV(const std::vector<size_t>&);

} // namespace NMath

namespace NTable {

// TableClientImpl interface for migrator
// Migrator should be able
// - PrepareDataQuery
// - Schedule some cb to be executed
class IMigratorClient {
public:
    virtual ~IMigratorClient() = default;
    virtual TAsyncPrepareQueryResult PrepareDataQuery(const TSession& session, const TString& query,
        const TPrepareDataQuerySettings& settings) = 0;
    virtual void ScheduleTaskUnsafe(std::function<void()>&& fn, TDuration timeout) = 0;
};

class TRequestMigrator {
public:
    // Set host to migrate session from.
    // If the target session set requests will be reprepared on the new session in background.
    // Real migration will be performed after DoCheckAndMigrate call
    // IMPORTANT: SetHost methods are not reentrant. Moreover new call of this methods allowed only if
    // future returned from previous call was set.
    // The value of future means number of requests migrated
    NThreading::TFuture<ui64> SetHost(const TString& host, TSession targetSession);
    NThreading::TFuture<ui64> SetHost(const TString& host);

    // Checks and perform migration if the session suitable to be removed from host.
    // Prepared requests will be migrated if target session was set along with host.
    // Returns false if session is not suitable or unable to get lock to start migration
    // Returns true if session is suitable in this case Unlink methos on the session is called
    // This methos is thread safe.
    bool DoCheckAndMigrate(TSession::TImpl* session, std::shared_ptr<IMigratorClient> client);

    // Wait current migration to be finished (if has one)
    void Wait() const;

    // Reset migrator to initiall state if migration was not started and returns true
    // Returns false if migration was started
    bool Reset();
private:
    std::function<void()> CreateMigrationTask();
    bool IsOurSession(TSession::TImpl* session) const;
    NThreading::TFuture<ui64> PrepareQuery(size_t id);

    TString CurHost_;
    std::vector<TString> Queries_;
    std::unique_ptr<TSession> TargetSession_;

    mutable std::mutex Lock_;
    NThreading::TPromise<ui64> Promise_;
    NThreading::TFuture<void> Finished_;
};

} // namespace NTable
} // namespace NYdb
