#include "shmem/SharedMemoryMasterProxy.h"

#include "shmem/ActorSharedMemory.h"
#include "actormsg/StopActorMessage.h"
#include "management/ActorServer.h"

#include "log.h"

namespace activebsp
{

SharedMemoryMasterProxy::
SharedMemoryMasterProxy(const std::shared_ptr<ActorCommunicator> & comm,
                        const std::shared_ptr<SharedMemoryRequestQueue> & queue,
                        const std::shared_ptr<ResultPublisher> & publisher,
                        const std::shared_ptr <ActorSharedMemory> & shmem)
    : _comm(comm), _queue(queue), _publisher(publisher), _shmem(shmem) {}

void SharedMemoryMasterProxy::reportResult(const ActiveObjectRequest & req, const std::vector<char> & data)
{
    _publisher->publishResult(req, data);
}

ActiveObjectRequest SharedMemoryMasterProxy::getNextRequest()
{
    return _queue->getNextMessage();
}

bool SharedMemoryMasterProxy::hasRequest()
{
    return !_queue->isEmpty();
}

void SharedMemoryMasterProxy::destroyMasterObject()
{
    _comm->sendActorMessage(_comm->pid(), StopActorMessage(false));
}

int SharedMemoryMasterProxy::readStoredResult(char * buf, int key, int offset, int size)
{
    LOG_TRACE("Preparing to read %d bytes at offset %d from part on shared memory ar key %d", size, offset, key);

    int res;

    START_MEASURE(1);
    ActorServer * actorServer = _shmem->getActorServer();
    pthread_mutex_t * lock = actorServer->getResultStoreLock();
    DataStore * resultStore = actorServer->getResultStore();
    pthread_cond_t * cv = actorServer->getResultStoreCV();
    END_MEASURE(1);
    LOG_MEASURE(1, "Aquiring storage lock");

    LOG_TRACE("%s", "Locking result store mutex");
    pthread_mutex_lock(lock);
    LOG_TRACE("%s", "Locked result store mutex");

    START_MEASURE(2);
    char * stored_data = NULL;
    size_t stored_data_size = 0;

    bool found = false;

    do
    {
        resultStore->getPartData(key, &stored_data, &stored_data_size);

        if (stored_data != NULL)
        {
            found = true;
        }
        else
        {
            pthread_cond_wait(cv, lock);
        }
    }
    while (!found);

    END_MEASURE(2);
    LOG_MEASURE(2, "Finding part in shared memory");

    if (stored_data == NULL)
    {
        res = -1;
    }
    else
    {
        if (size_t(offset + size) > stored_data_size)
        {
            std::cerr << "Tried to read " << size << " bytes at offset " << offset
                      << " from data item of key " << key << " while it has only " << stored_data_size << " bytes" << std::endl;

            res = -1;
        }
        else
        {
            START_MEASURE(3);
            memcpy(buf, stored_data + offset, size);
            res = 0;
            END_MEASURE(3);
            LOG_MEASURE(3, "Calling memcpy");
        }
    }

    LOG_TRACE("%s", "Unlocking result store mutex");
    pthread_mutex_unlock(lock);
    LOG_TRACE("%s", "Unkocked result store mutex");

    return res;
}

int SharedMemoryMasterProxy::writeStoredResult(char *, int, int)
{
    LOG_TRACE("%s", "WARNING: Calling function not implemented");

    /*
    ActorServer * actorServer = _shmem->getActorServer();
    pthread_mutex_t lock = actorServer->getResultStoreLock();
    auto resultStore = actorServer->getResultStore();

    pthread_mutex_lock(&lock);

    resultStore.emplace(msg->getResId(), msg);

    pthread_mutex_unlock(&lock);
    */
    return 0;
}

int SharedMemoryMasterProxy::getNPendingRequests()
{
    return _queue->getNPendingRequests();
}

} // namespace activebsp
