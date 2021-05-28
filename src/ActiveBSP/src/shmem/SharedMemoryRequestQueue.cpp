#include "shmem/SharedMemoryRequestQueue.h"

namespace activebsp
{

SharedMemoryRequestQueue::SharedMemoryRequestQueue()
{
    pthread_mutex_init(&_lock, NULL);
    pthread_cond_init(&_cv, NULL);
}

SharedMemoryRequestQueue::~SharedMemoryRequestQueue()
{
    pthread_cond_destroy(&_cv);
    pthread_mutex_destroy(&_lock);
}

ActiveObjectRequest SharedMemoryRequestQueue::getNextMessage()
{
    pthread_mutex_lock(&_lock);

    while (_messages.empty())
    {
        pthread_cond_wait(&_cv, &_lock);
    }

    ActiveObjectRequest req = _messages.front();
    _messages.pop();

    pthread_mutex_unlock(&_lock);

    return req;
}

void SharedMemoryRequestQueue::postMessage(const ActiveObjectRequest & request)
{
    pthread_mutex_lock(&_lock);

    _messages.push(request);

    pthread_cond_signal(&_cv);
    pthread_mutex_unlock(&_lock);
}

bool SharedMemoryRequestQueue::isEmpty()
{
    bool empty;

    pthread_mutex_lock(&_lock);
    empty = _messages.empty();
    pthread_mutex_unlock(&_lock);

    return empty;
}

int SharedMemoryRequestQueue::getNPendingRequests()
{
    int nmessages;

    pthread_mutex_lock(&_lock);
    nmessages = _messages.size();
    pthread_mutex_unlock(&_lock);

    return nmessages;
}

} // namespace activebsp
