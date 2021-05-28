#ifndef __FUTURE_KEY_MANAGER_H__
#define __FUTURE_KEY_MANAGER_H__

#include <vector>

#include <pthread.h>

namespace activebsp
{

class FutureKeyManager
{
private:
    static FutureKeyManager * _instance;

    int _next;

    pthread_mutex_t _mtx;

    FutureKeyManager();

public:
    static FutureKeyManager * getInstance();

    int takeKey();
};

} // namespace activebsp

#endif // __FUTURE_KEY_MANAGER_H__
