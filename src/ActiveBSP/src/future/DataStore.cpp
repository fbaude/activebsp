#include "future/DataStore.h"

namespace activebsp
{

DataStore::DataStore()
{
    pthread_mutex_init(&_lock, NULL);
}

DataStore::~DataStore()
{
    pthread_mutex_destroy(&_lock);
}

void DataStore::storePartMessage(int key, const std::shared_ptr<StoreResultMessage> & msg)
{
    LOG_TRACE("Inserting part of key %d in message store", key);

    pthread_mutex_lock(&_lock);

    _result_msg_store.insert(std::make_pair(key, msg));
    dv_part_t dv_part;
    dv_part.key = msg->getResId();
    dv_part.size = msg->getDataSize();
    dv_part.location = message_store;
    dv_part.remaining_bytes = 0;

    auto res = _dv_parts.insert(std::make_pair(key, dv_part));
    assert(res.second == true);

    pthread_mutex_unlock(&_lock);
}

void DataStore::storePartVector(int key, std::vector<char> && v)
{
    pthread_mutex_lock(&_lock);
    LOG_TRACE("Inserting part of key %d in vector store", key);

    auto res = _result_vector_store.insert(std::make_pair(key, v));
    assert(res.second == true);

    //_result_vector_store.emplace(key, v);
    auto data_it = _result_vector_store.find(key);

    dv_part_t dv_part;
    dv_part.key = key;
    dv_part.size = data_it->second.size();
    dv_part.location = vector_store;
    dv_part.remaining_bytes = 0;

    auto res2 = _dv_parts.insert(std::make_pair(key, dv_part));
    assert(res2.second == true);
    pthread_mutex_unlock(&_lock);
}

void DataStore::preparePartVector(int key, size_t size)
{    
    LOG_TRACE("Preparing part of key %d in vector store", key);
    pthread_mutex_lock(&_lock);

    auto parts_it = _dv_parts.find(key);

    assert (parts_it == _dv_parts.end());

    auto data_it = _result_vector_store.find(key);

    assert (data_it == _result_vector_store.end());

    std::vector<char> v(size);

    auto res = _result_vector_store.insert(std::make_pair(key, v));
    assert(res.second == true);

    //_result_vector_store.emplace(key, std::move(v));
    data_it = _result_vector_store.find(key);

    dv_part_t dv_part;
    dv_part.key = key;
    dv_part.location = vector_store;
    dv_part.remaining_bytes = size;
    dv_part.size = data_it->second.size();

    auto res2 = _dv_parts.insert(std::make_pair(key, dv_part));
    assert(res2.second == true);

    pthread_mutex_unlock(&_lock);
}

char * DataStore::findDataBuf(int key)
{
    auto part_it = _dv_parts.find(key);

    assert(part_it != _dv_parts.end());

    char * data = NULL;

    if (part_it->second.location == vector_store)
    {
        auto data_it = _result_vector_store.find(key);

        assert(data_it != _result_vector_store.end());

        data = data_it->second.data();
    }
    else if (part_it->second.location == message_store)
    {
        auto data_it = _result_msg_store.find(key);

        assert(data_it != _result_msg_store.end());

        data = data_it->second->getData();
    }
    else
    {
        assert(false);
    }

    return data;
}

size_t DataStore::addData(int key, char * data, size_t offset, size_t size)
{
    LOG_TRACE("Adding data part of key %d and of size %zu at offset %zu", key, size, offset);
    pthread_mutex_lock(&_lock);

    auto it = _dv_parts.find(key);

    assert(it != _dv_parts.end());

    dv_part_t & part = it->second;
    char * buf = findDataBuf(key);

    assert(buf != NULL);

    assert (part.size >= offset + size);

    assert (size <= part.remaining_bytes);

    LOG_TRACE("Copying %zu bytes at offset %zu inside data buffer at address %p of key %d and size %zu", size, offset, buf, key, part.size);

    memcpy(buf + offset, data, size);
    part.remaining_bytes -= size;

    LOG_TRACE("%zu bytes remaining", part.remaining_bytes);

    pthread_mutex_unlock(&_lock);

    return part.remaining_bytes;
}

void DataStore::removePart(int key)
{
    LOG_TRACE("Removing part of key %d", key);

    pthread_mutex_lock(&_lock);

    auto part_it = _dv_parts.find(key);

    assert (part_it != _dv_parts.end());

    if (part_it->second.location == message_store)
    {
        auto store_it = _result_msg_store.find(key);
        assert(store_it != _result_msg_store.end());

        _result_msg_store.erase(store_it);
    }
    else if (part_it->second.location == vector_store)
    {
        auto store_it = _result_vector_store.find(key);
        assert (store_it != _result_vector_store.end());

        _result_vector_store.erase(store_it);
    }
    else
    {
        assert(false);
    }

    _dv_parts.erase(part_it);

    pthread_mutex_unlock(&_lock);
}

void DataStore::getPartData(int key, char ** data, size_t * size)
{
    pthread_mutex_lock(&_lock);

    auto it = _dv_parts.find(key);

    if (it == _dv_parts.end())
    {
        *data = NULL;
        *size = 0;
        LOG_TRACE("Part data of key %d not found", key);

        pthread_mutex_unlock(&_lock);
        return;
    }

    if (it == _dv_parts.end() || it->second.remaining_bytes > 0)
    {
        *data = NULL;
        *size = 0;
        LOG_TRACE("Part data of key %d found but needs %zu more bytes to be ready", key, it->second.remaining_bytes);

        pthread_mutex_unlock(&_lock);
        return;
    }

    const dv_part_t & part = it->second;

    *data = findDataBuf(key);
    *size = part.size;

    pthread_mutex_unlock(&_lock);
}


} // namespace activebsp
