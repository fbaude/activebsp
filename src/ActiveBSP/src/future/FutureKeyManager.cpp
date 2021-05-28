#include "future/FutureKeyManager.h"

#include <cstddef>

#include <algorithm>
#include <iostream>

#include "actormsg/activebsp_tags.h"

namespace activebsp
{

FutureKeyManager::FutureKeyManager()
    : _next(0)
{
    pthread_mutex_init(&_mtx, NULL);
}

int FutureKeyManager::takeKey()
{
    pthread_mutex_lock(&_mtx);
    int key = _next++;
    pthread_mutex_unlock(&_mtx);

    return key;
}

FutureKeyManager * FutureKeyManager::_instance = NULL;

FutureKeyManager * FutureKeyManager::getInstance()
{
    if (_instance == NULL)
    {
        _instance = new FutureKeyManager();
    }

    return _instance;
}

} // namespace activebsp
