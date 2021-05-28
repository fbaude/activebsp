#include "management/ActorServer.h"

#include "future/FutureKeyManager.h"
#include "log.h"
#include "distributions/block_distribution.h"

#define MEASURE_AVAILABILITY 1

#include "measure.h"

namespace activebsp
{

ActorServer::ActorServer(const std::shared_ptr<ActorCommunicator> & actorComm,
                         const std::shared_ptr<ActorWorkerFactory> & actorWorkerFactory)
    : _actorComm(actorComm), _actorWorkerFactory(actorWorkerFactory)
{
    pthread_mutex_init(&_result_store_lock, NULL);
    pthread_cond_init(&_result_store_cv, NULL);
}

ActorServer::~ActorServer() {}

void ActorServer::setIntraActorComm(const std::shared_ptr<IntraActorCommunicator> & intraActorComm)
{
    _intraActorComm = intraActorComm;
}

void ActorServer::routeHandleMessage(const std::shared_ptr<ActorMessage> & msg)
{
    switch (msg->getInstruction())
    {
    case INSTRUCTION_STOP:
        handleStopActorProcessMessage(std::static_pointer_cast<StopActorProcessMessage>(msg));
        break;
    case INSTRUCTION_CREATE_ACTOR:
        handleCreateActorMessage(std::static_pointer_cast<CreateActorMessage>(msg));
        break;
    case INSTRUCTION_CALL_ACTOR:
        handleCallActorMessage(std::static_pointer_cast<CallActorMessage>(msg));
        break;
    case INSTRUCTION_DESTROY_ACTOR:
        handleStopActorMessage(std::static_pointer_cast<StopActorMessage>(msg));
        break;
    case INSTRUCTION_REGISTER_FUTURE_DATA:
        handleRegisterFuture(std::static_pointer_cast<RegisterFutureMessage>(msg));
        break;
    case INSTRUCTION_GET_FUTURE_DATA:
        handleGetFutureData(std::static_pointer_cast<GetFutureDataMessage>(msg));
        break;
    case INSTRUCTION_RELEASE_FUTURE:
        handleReleaseFutureMessage(std::static_pointer_cast<ReleaseFutureMessage>(msg));
        break;
    case INSTRUCTION_GET_RESULT_PARTS:
        handleGetResultPartsMessage(std::static_pointer_cast<GetResultPartsMessage>(msg));
        break;
    case INSTRUCTION_STORE_RESULT:
        handleStoreResultMessage(std::static_pointer_cast<StoreResultMessage>(msg));
        break;
    case INSTRUCTION_RELEASE_RESULT:
        handleReleaseResultPartMessage(std::static_pointer_cast<ReleaseResultPartMessage>(msg));
        break;
    case INSTRUCTION_DISTRIBUTE_VECTOR:
        handleDistributeVectorMessage(std::static_pointer_cast<DistributeVectorMessage>(msg));
        break;
    case INSTRUCTION_PREFETCH:
        handlePrefetchMessage(std::static_pointer_cast<PrefetchMessage>(msg));
        break;
    case INSTRUCTION_FETCH:
        handleFetchMessage(std::static_pointer_cast<FetchMessage>(msg));
        break;
    case INSTRUCTION_STORE_RESULT_PART:
        handleStoreResultPartMessage(std::static_pointer_cast<StoreResultPartMessage>(msg));
        break;
    default:
        std::cout << "Process received unknown instruction" << std::endl;
        break;
    }
}


void ActorServer::handleCreateActorMessage(const std::shared_ptr <CreateActorMessage> & msg)
{
    _pids = msg->getPids();
    _actorWorkerProxy = _actorWorkerFactory->createActorProcessProxy(this);
    int errorcode = _actorWorkerProxy->startActor(msg->getActorName(), _pids);

    if (_actorComm->pid() == _pids[0])
    {
        _actorComm->sendResponseCode(msg->getSrc(), errorcode);
    }
}

void ActorServer::handleCallActorMessage(const std::shared_ptr <CallActorMessage> & msg)
{
    LOG_TRACE("%s", "Handling CallActor message");

    START_MEASURE(1);

    int key = FutureKeyManager::getInstance()->takeKey();

    LOG_TRACE("Chose key %d for future", key);

    START_MEASURE(2);
    ActiveObjectRequest req (msg, msg->getSrc(), key);
    END_MEASURE(2);

    LOG_TRACE("%s", "Queing request");

    START_MEASURE(3);
    _actorWorkerProxy->callActor(req);
    END_MEASURE(3);

    START_MEASURE(4);
    _actorComm->sendResponseCode(msg->getSrc(), key);
    END_MEASURE(4);

    END_MEASURE(1);

    LOG_MEASURE(2, "Creating active object request");
    LOG_MEASURE(3, "Sending request to worker");
    LOG_MEASURE(4, "Sending response code to caller");
    LOG_MEASURE(1, "Handling CallActorMessage");
}

void ActorServer::handleStopActorMessage(const std::shared_ptr <StopActorMessage> & msg)
{
    _pids.clear();
    _actorWorkerProxy->stopActor();
    _actorWorkerProxy.reset();
    //_result_store.clear();

    if (msg->isAskingAnswer())
    {
        _actorComm->sendResponseCode(msg->getSrc(), 0);
    }
}

void ActorServer::handleStopActorProcessMessage(const std::shared_ptr <StopActorProcessMessage> & msg)
{
    UNUSED(msg); // No information required, but could be in more elaborate implementation
    _qrun = false;
}

void ActorServer::registerFuture(int key, std::vector<char> && data)
{
    LOG_TRACE("Registering future of key %d and size %zu", key, data.size());

    START_MEASURE(1);

    auto pendingIt = _pendingFutureRequests.find(key);

    if (pendingIt == _pendingFutureRequests.end() && _askedFutureDelete[key])
    {
        LOG_TRACE("Future of key %d was requested deletion before its data arrived, forgetting about it", key);

        _askedFutureDelete.erase(key);
    }
    else
    {
        LOG_TRACE("Writing future of key %d to store", key);

        START_MEASURE(2);

        _futureDataStore.registerFutureData(key, data);

        END_MEASURE(2);
        LOG_MEASURE(2, "Storing future data");

        std::vector<char> * data = _futureDataStore.getFutureData(key);

        checkFutureRequired(key);

        if (pendingIt != _pendingFutureRequests.end())
        {
            LOG_TRACE("%s", "Future has pending requests");

            for (auto pidit = pendingIt->second.begin(); pidit != pendingIt->second.end(); ++pidit)
            {
                LOG_TRACE("Sending future data to P%d", *pidit);

                START_MEASURE(3);
                _actorComm->sendFutureData(*pidit, *data);
                END_MEASURE(3);
                LOG_MEASURE(3, "Sending future data");
            }

            _pendingFutureRequests.erase(pendingIt);
        }

        if (_askedFutureDelete[key])
        {
            _askedFutureDelete.erase(key);
            _futureDataStore.removeFutureData(key);
        }
    }

    END_MEASURE(1);
    LOG_MEASURE(1, "Handling RegisterFuture message");

    LOG_TRACE("finished registering future of key %d", key);
}

void ActorServer::handleRegisterFuture(const std::shared_ptr <RegisterFutureMessage> & msg)
{
    LOG_TRACE("%s", "Handling RegisterFuture message");

    registerFuture(msg->getFutureKey(), msg->getFutureData());
}

void ActorServer::handleGetFutureData(const std::shared_ptr <GetFutureDataMessage> & msg)
{
    LOG_TRACE("%s", "Handling GetFutureData message");
    START_MEASURE(1);

    int key = msg->getFutureKey();
    int src = msg->getSrc();

    std::vector<char> * data = _futureDataStore.getFutureData(key);
    if (data)
    {
        LOG_TRACE("Data of key %d found, sending it to P%d", key, src);

        _actorComm->sendFutureData(src, *data);
    }
    else
    {
        LOG_TRACE("Data of key %d not found, queuing request from P%d", key, src);

        _pendingFutureRequests[key].push_back(src);
    }

    END_MEASURE(1);
    LOG_MEASURE(1, "Hahdling GetFutureData message");
}

void ActorServer::handleReleaseFutureMessage(const std::shared_ptr <ReleaseFutureMessage> & msg)
{
    int key = msg->getFutureKey();
    std::vector<char> * data = _futureDataStore.getFutureData(key);

    if (data != NULL)
    {
        _futureDataStore.removeFutureData(msg->getFutureKey());
    }
    else
    {
        _askedFutureDelete[key] = true;
    }
}

void ActorServer::handleGetResultPartsMessage(const std::shared_ptr <GetResultPartsMessage> & msg)
{
    LOG_TRACE("%s", "Get result part");

    START_MEASURE(1);

    LOG_TRACE("%s", "Locking result store mutex");
    pthread_mutex_lock(&_result_store_lock);
    LOG_TRACE("%s", "Locked result store mutex");

    int nparts = msg->getNparts();

    for (int i = 0; i < nparts; ++i)
    {
        LOG_TRACE("Retreiving part %d", i);

        START_MEASURE(2);

        const result_req_part_t * part = msg->getPart(i);

        char * data;
        size_t data_size;

        _dataStore.getPartData(part->resid, &data, &data_size);

        if (data == NULL)
        {
            LOG_TRACE("%s", "Result not available, storing request for when it is");

            _pendingResultRequests[part->resid].push_back({part->offset, part->size, msg->getSrc()});

            continue;
        }

        //std::shared_ptr<StoreResultMessage> & stored_msg = it_msg->second;
        //if (part->offset + part->size > stored_msg->getDataSize())
        if (part->offset + part->size > data_size)
        {
            std::cout << "p" << _actorComm->pid() << " was requested out of bounds in part of id " << part->resid
                      << ", part has size " << data_size << " but offset of " << part->offset
                      << " and size of " << part->size << " were requested by p" << msg->getSrc() << std::endl;

            return;
        }

        //const char * res = stored_msg->getData();
        const char * res = data;
        res += part->offset;

        LOG_TRACE("p%d sending part of id %d, offset %zu and size %zu to p%d", _actorComm->pid(), part->resid, part->offset, part->size, msg->getSrc());

        START_MEASURE(3);

        _actorComm->sendResultPart(msg->getForwardTo(), res, part->size);

        END_MEASURE(3);
        LOG_MEASURE(3, "sending part");

        END_MEASURE(2);
        LOG_MEASURE(2, "Handling part");
    }

    LOG_TRACE("%s", "Unlocking result store mutex");
    pthread_mutex_unlock(&_result_store_lock);
    LOG_TRACE("%s", "Unlocked result store mutex");

    END_MEASURE(1);
    LOG_MEASURE(1, "get result part");
}

void ActorServer::handleStoreResultMessage(const std::shared_ptr <StoreResultMessage> & msg)
{
    LOG_TRACE("%s", "Storing result");

    START_MEASURE(3);
    LOG_TRACE("%s", "Giving buffer ownership");

    _actorComm->giveBufferOwnershipTo(*msg);
    LOG_TRACE("%s", "Gave buffer ownership");

    END_MEASURE(3);
    LOG_MEASURE(3, "Taking receive buffer ownsership and replacing it");

    START_MEASURE(1);

    LOG_TRACE("%s", "Locking result store mutex");

    pthread_mutex_lock(&_result_store_lock);

    LOG_TRACE("%s", "Unlocking result store mutex");

    _dataStore.storePartMessage(msg->getResId(), msg);

    pthread_cond_broadcast(&_result_store_cv);

    pthread_mutex_unlock(&_result_store_lock);

    END_MEASURE(1);
    LOG_MEASURE(1, "inserting result");

    LOG_TRACE("%s", "Stored result");

    checkDvPartRequired(msg->getResId());

    LOG_TRACE("%s", "Finished handling store result message");
}

void ActorServer::checkDvPartRequired(int key)
{
    LOG_TRACE("Checking if dv part of key %d is required", key);

    START_MEASURE(2);

    auto it = _pendingResultRequests.find(key);
    if (it != _pendingResultRequests.end())
    {
        LOG_TRACE("%s", "Sending result to pending requests");

        pthread_mutex_lock(&_result_store_lock);
        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            LOG_TRACE("Sending result to pending request from p%d", it2->dst);

//            auto result_entry = _result_msg_store.find(it->first);
            //std::shared_ptr<StoreResultMessage> & stored_msg = result_entry->second;
            //const char * res = stored_msg->getData();

            char * data;
            size_t data_size;
            _dataStore.getPartData(key, &data, &data_size);

            const char * res = data;
            res += it2->offset;

            //const char * res = &_result_store[it->first][0];
            //res += it2->offset;
            _actorComm->sendResultPart(it2->dst, res, it2->size);
        }
        pthread_mutex_unlock(&_result_store_lock);
    }

    END_MEASURE(2);
    LOG_MEASURE(2, "fullfilling pending requests");
}

void ActorServer::handleReleaseResultPartMessage(const std::shared_ptr <ReleaseResultPartMessage> & msg)
{
    int resid = msg->getResId();

    pthread_mutex_lock(&_result_store_lock);

    /*
    auto it = _result_store.find(resid);
    if (it != _result_store.end())
    {
        _result_store.erase(it);
    }
    */
    _dataStore.removePart(resid);

    pthread_mutex_unlock(&_result_store_lock);

}

void ActorServer::handleDistributeVectorMessage(const std::shared_ptr <DistributeVectorMessage> & msg)
{
    int src = msg->getSrc();

    // Only block distribution for now
    size_t n = ((block_distr_args_t *) msg->getDistrArgs())->n;
    size_t elem_size = ((block_distr_args_t *) msg->getDistrArgs())->elem_size;

    int p = _pids.size();

    vector_distribution_base dv(p);

    for (int i = 0; i < p; ++i)
    {
        int key = _next_result_store_local_key--;

        size_t offset, end, size;
        div_range(n, i, p, &offset, &end);
        size = end - offset;

        dv.register_part(i, _pids[i], key, size * elem_size, offset * elem_size);
    }

    _actorComm->sendDistrVector(src, dv);
}

void ActorServer::handlePrefetchMessage(const std::shared_ptr <PrefetchMessage> & msg)
{
    LOG_TRACE("%s", "Handling prefetch message");

    int res_future_key;
    if (isHead())
    {
        LOG_TRACE("%s", "Handling prefetch message as head process");

        // Reply new future key
        res_future_key = FutureKeyManager::getInstance()->takeKey();
        _actorComm->sendResponseCode(msg->getSrc(), res_future_key);

        // Broadcast message to other processes
        // TODO make it asynchronous/Waitall
        for (auto pid_it = _pids.begin() + 1; pid_it != _pids.end(); ++pid_it)
        {
            _actorComm->sendActorMessage(*pid_it, *msg);
        }
    }
    else
    {
        LOG_TRACE("%s", "Handling prefetch message as slave process");

    }

    // Deduce size
    int s,p;
    size_t size, part_size, part_offset, part_end;
    size = msg->getDvSize();

    s = _intraActorComm->pid();
    p = _intraActorComm->nprocs();
    div_range(size, s, p, &part_offset, &part_end);
    part_size = part_end - part_offset;

    // get key for future parts
    int part_key = FutureKeyManager::getInstance()->takeKey();

    // Prepare buffers

    pthread_mutex_lock(&_result_store_lock);

    LOG_TRACE("Registering %zu missing bytes for part of key %d", part_size, part_key);
    _partMissing[part_key] = part_size;

    LOG_TRACE("Allocating %zu bytes for part of key %d", part_size, part_key);
    _dataStore.preparePartVector(part_key, part_size);

    pthread_mutex_unlock(&_result_store_lock);

    // Gather keys, offsets and sizes to head process message

    if (isHead())
    {
        FetchMessage fetchMessage(_pids.data(), _pids.size(), msg->getFutureKey(), res_future_key, size);

        LOG_TRACE("%s", "Gathering keys into getch message buffer");

        _intraActorComm->gatherKeys(fetchMessage.getKeys(), part_key);

        LOG_TRACE("Sending fetch message to p%d", msg->getFuturePid());

        _actorComm->sendActorMessage(msg->getFuturePid(), fetchMessage);

        // Compute vector distribution and register it as future

        vector_distribution_base vd(p);

        for (int i = 0; i < p; ++i)
        {
            size_t i_part_offset, i_part_size, i_part_end;
            div_range(size, i, p, &i_part_offset, &i_part_end);
            i_part_size = i_part_end - i_part_offset;

            vd.register_part(i, fetchMessage.getPids()[i], fetchMessage.getKeys()[i], i_part_size, i_part_offset);
        }

        registerFuture(res_future_key, serialize_all_to_vchar(vd));
    }
    else
    {
        _intraActorComm->gatherKeys(NULL, part_key);
    }

}

void ActorServer::handleFetchMessage(const std::shared_ptr <FetchMessage> & msg)
{
    LOG_TRACE("%s", "Handling fetch message");

    if (isHead())
    {
        std::vector<char> * dv_future = _futureDataStore.getFutureData(msg->getReqFutureKey());

        if (dv_future == NULL)
        {
            LOG_TRACE("Future of key %d was not available, saving request", msg->getReqFutureKey());

            auto inserted = _askedFetch.insert(std::make_pair(msg->getReqFutureKey(), std::make_shared<FetchMessage>(*msg)));
            assert(inserted.second == true);

            return;
        }
    }

    fetchDv(msg);
}

void ActorServer::fetchDv(const std::shared_ptr <FetchMessage> & msg)
{
    LOG_TRACE("%s", "Forwarding dv");

    vector_distribution_base dv;

    if (isHead())
    {
        for (size_t i = 1; i < _pids.size(); ++i)
        {
            _actorComm->sendActorMessage(_pids[i], *msg);
        }

        std::vector<char> * dv_future = _futureDataStore.getFutureData(msg->getReqFutureKey());
        deserialize_all(dv_future->data(), dv_future->size(), dv);
    }

    _intraActorComm->broadcastVd(dv);

    int npids = msg->getNpids();
    size_t size = msg->getDvSize();
    int nparts = dv.nparts();

    for (int i = 0; i < npids; ++i)
    {
        int pid = msg->getPids()[i];
        int key = msg->getKeys()[i];

        size_t ioffset, iend;
        div_range(size, i, npids, &ioffset, &iend);

        for (int i = 0; i < nparts; ++i)
        {
            const vector_distribution_part_t * part = dv.getPart(i);
            size_t part_end;

            if (_actorComm->pid() != part->pid) continue;

            part_end = part->offset + part->size;

            // If this dv part finishes before the part we are interested in
            // or if it starts after it, then it not a subset of the part we want
            if (part_end <= ioffset || part->offset >= iend) continue;

            pthread_mutex_lock(&_result_store_lock);

            char * data;
            size_t data_size;
            _dataStore.getPartData(part->resid, &data, &data_size);

            // Send subpart
            // Assume data is already there, just print a warning if not
            if (data == NULL)
            {
                LOG_TRACE("WARNING: did not find data of key %d but putting it in waiting list not implemented. Assuming it is there", part->resid);
            }

            assert (data != NULL);

            size_t req_offset = ioffset >= part->offset ? ioffset - part->offset : 0;
            size_t req_size = std::min( part->size, iend - part->offset) - req_offset;

            LOG_TRACE("Sending StoreResultPart message to %d with data of key %d, offset %zu and size %zu", pid, key, 0ul, req_size);

            StoreResultPartMessage msg(key, data, 0, req_size, size);
            _actorComm->sendActorMessage(pid, msg);

            pthread_mutex_unlock(&_result_store_lock);
        }
    }
}

void ActorServer::handleStoreResultPartMessage(const std::shared_ptr <StoreResultPartMessage> & msg)
{
    LOG_TRACE("%s", "Handling StoreResultPart message");

    LOG_TRACE("Adding %zu bytes to data store item of key %d at offset %zu", msg->getDataSize(), msg->getResId(), msg->getOffset());
    pthread_mutex_lock(&_result_store_lock);

    size_t remaining = _dataStore.addData(msg->getResId(), msg->getData(), msg->getOffset(), msg->getDataSize());

    pthread_cond_broadcast(&_result_store_cv);

    pthread_mutex_unlock(&_result_store_lock);

    LOG_TRACE("Added %zu bytes, %zu remaining to have the full part", msg->getDataSize(), remaining);

    if (remaining == 0)
    {
        checkDvPartRequired(msg->getResId());
    }
}

void ActorServer::checkFutureRequired(int key)
{
    LOG_TRACE("Checking if future of key %d was is requested", key);

    auto fetchMsgIt = _askedFetch.find(key);

    if (fetchMsgIt != _askedFetch.end())
    {
        LOG_TRACE("Future of key %d was requested", key);

        fetchDv(fetchMsgIt->second);

        _askedFetch.erase(fetchMsgIt);
    }
}


bool ActorServer::isHead() const
{
    return _intraActorComm->pid() == 0;
}


void ActorServer::run()
{
#if MEASURE_AVAILABILITY
    struct timespec t0,t1;
    double time_working,time_waiting;

    time_working = 0.0;
    time_waiting = 0.0;

    t0 = now();
#endif

    _qrun = true;
    while (_qrun)
    {
        std::shared_ptr<ActorMessage> message = _actorComm->receiveActorMessage();

#if MEASURE_AVAILABILITY
        t1 = now();
        time_waiting += diff_to_sec(&t0,&t1);
        t0 = t1;
#endif

        LOG_TRACE("%s", "Management thread received message");

        if (message == nullptr)
        {
            std::cout << "p" << _actorComm->pid() << " received unknown message" << std::endl;
            continue;
        }

        START_MEASURE(1);

        routeHandleMessage(message);

#if MEASURE_AVAILABILITY
        t1 = now();
        time_working += diff_to_sec(&t0,&t1);
        t0 = t1;
#endif

        END_MEASURE(1);
        LOG_MEASURE(1, "management thread serving message");
    }

    if (_actorWorkerProxy)
    {
        std::cerr << "Process exiting without destroying owned actor first. Doing cleanup" << std::endl;
        _actorWorkerProxy->stopActor();
        _actorWorkerProxy.reset();
    }

    LOG_TRACE("P%d exiting\n", _actorComm->pid());

#if ENABLE_LOG_MEASURE
    double percent = (time_working / (time_working + time_waiting)) * 100.00;
    printf("Management thread of P%d was working %f%% of the time\n", _actorComm->pid(), percent);
#endif
}

pthread_mutex_t * ActorServer::getResultStoreLock()
{
    return &_result_store_lock;
}

pthread_cond_t * ActorServer::getResultStoreCV()
{
    return &_result_store_cv;
}

DataStore *ActorServer::getResultStore()
{
    return &_dataStore;
}

} // namespace activebsp
