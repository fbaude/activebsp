#include "future/LazyFutureDataStore.h"

#include <iostream>

namespace activebsp
{

void LazyFutureDataStore::registerFutureData(future_key_t key, const std::vector<char> & data)
{
    _store[key] = data;
}

void LazyFutureDataStore::registerFutureData(future_key_t key, std::vector<char> && data)
{
    _store.emplace(key, data);
}

std::vector<char> * LazyFutureDataStore::getFutureData(future_key_t key)
{
    auto it = _store.find(key);

    if (it == _store.end())
    {
        return NULL;
    }

    return &it->second;
}

void LazyFutureDataStore::removeFutureData(future_key_t key)
{    
    auto it = _store.find(key);

    if (it != _store.end()) {
        _store.erase(key);
    }
    else
    {
        std::cerr << "Asked to delete unknown future of key " << key << std::endl;
    }
}

} // namespace activebsp
