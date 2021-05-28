#ifndef __SHARED_MEMORY_MASTER_PROXY_H__
#define __SHARED_MEMORY_MASTER_PROXY_H__

#include <memory>

#include "management/MasterProxy.h"
#include "SharedMemoryRequestQueue.h"
#include "future/ResultPublisher.h"
#include "actormsg/ActiveObjectRequest.h"
#include "ActorCommunicator.h"
#include "ActorSharedMemory.h"

namespace activebsp
{

class SharedMemoryMasterProxy : public MasterProxy
{
private:
    std::shared_ptr<ActorCommunicator> _comm;
    std::shared_ptr<SharedMemoryRequestQueue> _queue;
    std::shared_ptr<ResultPublisher> _publisher;
    std::shared_ptr <ActorSharedMemory> _shmem;

public:
    SharedMemoryMasterProxy(const std::shared_ptr<ActorCommunicator> & comm,
                            const std::shared_ptr<SharedMemoryRequestQueue> & queue,
                            const std::shared_ptr<ResultPublisher> & publisher,
                            const std::shared_ptr <ActorSharedMemory> & shmem);

    virtual void reportResult(const ActiveObjectRequest & req, const std::vector<char> & data);
    virtual ActiveObjectRequest getNextRequest();
    virtual bool hasRequest();
    virtual void destroyMasterObject();

    virtual int readStoredResult(char * buf, int key, int offset, int size);
    virtual int writeStoredResult(char * buf, int key, int size);

    virtual int getNPendingRequests();
};

} // namespace activebsp

#endif // __SHARED_MEMORY_MASTER_PROXY_H__
