#ifndef __LAZY_FUTURE_DATA_STORE_H__
#define __LAZY_FUTURE_DATA_STORE_H__

#include <map>
#include <vector>

namespace activebsp
{

class LazyFutureDataStore
{
public:
    typedef int future_key_t;

private:
    std::map<future_key_t, std::vector<char> > _store;

    public:
        void registerFutureData(future_key_t key, const std::vector<char> & data);
        void registerFutureData(future_key_t key, std::vector<char> && data);
        std::vector<char> *getFutureData(future_key_t key);
        void removeFutureData(future_key_t key);
};

} // namespace activebsp

#endif // __LAZY_FUTURE_DATA_STORE_H__
