#ifndef DATASTORE_H
#define DATASTORE_H

#include <list>
#include <map>
#include <vector>
#include <memory>

#include "actormsg/StoreResultMessage.h"

#include "log.h"

namespace activebsp
{

class DataStore
{
private:
    typedef std::map<int, std::shared_ptr<StoreResultMessage> > msg_store_t;
    typedef std::map<int, std::vector<char> > vector_store_t;

    vector_store_t _result_vector_store;
    msg_store_t _result_msg_store;

    pthread_mutex_t _lock;

    typedef enum
    {
        vector_store,
        message_store

    } part_location_t;

    typedef struct dv_part_t
    {
        int key;
        size_t size;
        part_location_t location;
        size_t remaining_bytes;

    } dv_part_t;

    std::map <int, dv_part_t> _dv_parts;

    char * findDataBuf(int key);

public:
    DataStore();
    ~DataStore();

    void storePartMessage(int key, const std::shared_ptr<StoreResultMessage> & msg);
    void storePartVector(int key, std::vector<char> && v);

    void preparePartVector(int key, size_t size);
    size_t addData(int key, char * data, size_t offset, size_t size);

    void removePart(int key);

    void getPartData(int key, char **data, size_t * size);

};

} // namespace activebsp

#endif // DATASTORE_H
