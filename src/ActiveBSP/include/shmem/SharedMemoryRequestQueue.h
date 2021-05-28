#ifndef __SHARED_MEMORY_REQUEST_QUEUE_H__
#define __SHARED_MEMORY_REQUEST_QUEUE_H__

#include <queue>

#include <pthread.h>

#include "actormsg/ActiveObjectRequest.h"

namespace activebsp
{

class SharedMemoryRequestQueue
{
private:
    pthread_mutex_t _lock;
    pthread_cond_t _cv;

    std::queue<ActiveObjectRequest> _messages;

public:
    SharedMemoryRequestQueue();
    ~SharedMemoryRequestQueue();

    ActiveObjectRequest getNextMessage();
    void postMessage(const ActiveObjectRequest &request);

    bool isEmpty();
    int getNPendingRequests();
};

} // namespace activebsp

#endif // __SHARED_MEMORY_REQUEST_QUEUE_H__
