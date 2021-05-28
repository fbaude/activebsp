#ifndef __ACTOR_SERVER_H__
#define __ACTOR_SERVER_H__

#include <memory>

#include "Actor.h"
#include "ActorCommunicator.h"
#include "mpi/MPIWrapper.h"
#include "worker/ActorWorkerFactory.h"
#include "worker/ActorWorkerProxy.h"
#include "IntraActorCommunicator.h"
#include "IntraActorCommunicatorFactory.h"
#include "future/LazyFutureDataStore.h"
#include "future/DataStore.h"

// Breaking abstraction
#include <pthread.h>

#include "actormsg/CreateActorMessage.h"
#include "actormsg/CallActorMessage.h"
#include "actormsg/StopActorMessage.h"
#include "actormsg/StopActorProcessMessage.h"
#include "actormsg/RegisterFutureMessage.h"
#include "actormsg/GetFutureDataMessage.h"
#include "actormsg/ReleaseFutureMessage.h"
#include "actormsg/GetResultPartsMessage.h"
#include "actormsg/StoreResultMessage.h"
#include "actormsg/ReleaseResultPartMessage.h"
#include "actormsg/DistributeVectorMessage.h"
#include "actormsg/PrefetchMessage.h"
#include "actormsg/FetchMessage.h"
#include "actormsg/StoreResultPartMessage.h"

namespace activebsp
{

class ActorServer
{
private:
    std::shared_ptr<ActorCommunicator> _actorComm;
    std::shared_ptr<ActorWorkerFactory> _actorWorkerFactory;
    std::shared_ptr<IntraActorCommunicator> _intraActorComm;

    std::shared_ptr<ActorWorkerProxy> _actorWorkerProxy;

    std::vector<int> _pids;

    LazyFutureDataStore _futureDataStore;
    std::map<int,std::vector<int>> _pendingFutureRequests;
    std::map<int,bool> _askedFutureDelete;

    std::map<int, std::shared_ptr<FetchMessage> > _askedFetch;
    std::map<int, size_t> _partMissing;

    typedef struct {
        size_t offset;
        size_t size;
        int dst;
    } pending_result_request_t;

    pthread_mutex_t _result_store_lock;
    pthread_cond_t _result_store_cv;

    DataStore _dataStore;

    typedef struct {
        char * data;
        size_t size;
    } result_item_t;

    std::map<int, result_item_t> _result_items;

    std::map<int,std::vector<pending_result_request_t>> _pendingResultRequests;
    int _next_result_store_local_key = -1;

    std::map<int, bool> _pendingPrefetchPart;

    bool _qrun;

    void checkFutureRequired(int key);
    void checkDvPartRequired(int key);

    void fetchDv(const std::shared_ptr <FetchMessage> & msg);


public:
    ActorServer(const std::shared_ptr<ActorCommunicator>&,
                const std::shared_ptr<ActorWorkerFactory>&);

    ~ActorServer();

    void setIntraActorComm(const std::shared_ptr<IntraActorCommunicator> & intraActorComm);

public:
    pthread_mutex_t * getResultStoreLock();
    pthread_cond_t * getResultStoreCV();
    DataStore * getResultStore();
public:
    bool isHead() const;

    void registerFuture(int key, std::vector<char> && data);

    void run();

    void routeHandleMessage(const std::shared_ptr<ActorMessage> & msg);
    void handleCreateActorMessage(const std::shared_ptr <CreateActorMessage> & msg);
    void handleCallActorMessage(const std::shared_ptr <CallActorMessage> & msg);
    void handleStopActorMessage(const std::shared_ptr <StopActorMessage> & msg);
    void handleStopActorProcessMessage(const std::shared_ptr <StopActorProcessMessage> & msg);
    void handleRegisterFuture(const std::shared_ptr <RegisterFutureMessage> & msg);
    void handleGetFutureData(const std::shared_ptr <GetFutureDataMessage> & msg);
    void handleReleaseFutureMessage(const std::shared_ptr <ReleaseFutureMessage> & msg);
    void handleGetResultPartsMessage(const std::shared_ptr <GetResultPartsMessage> & msg);
    void handleStoreResultMessage(const std::shared_ptr<StoreResultMessage> &msg);
    void handleReleaseResultPartMessage(const std::shared_ptr <ReleaseResultPartMessage> & msg);
    void handleDistributeVectorMessage(const std::shared_ptr <DistributeVectorMessage> & msg);
    void handlePrefetchMessage(const std::shared_ptr <PrefetchMessage> & msg);
    void handleFetchMessage(const std::shared_ptr <FetchMessage> & msg);
    void handleStoreResultPartMessage(const std::shared_ptr <StoreResultPartMessage> & msg);
};

} // namespace activebsp

#endif // __ACTOR_SERVER_H__
