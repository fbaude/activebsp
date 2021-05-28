#include "future/DirectFutureDataRetriever.h"

#include "future/FutureKeyManager.h"

namespace activebsp
{

DirectFutureDataRetriever::DirectFutureDataRetriever(const std::shared_ptr<ActorCommunicator> & comm, int src, int key)
    : _comm(comm), _src(src), _key(key) {}

DirectFutureDataRetriever::~DirectFutureDataRetriever() {}

std::shared_ptr<std::vector<char>> DirectFutureDataRetriever::retrieveData()
{
    std::shared_ptr<std::vector<char>> data = _comm->receiveKeyedFutureValue(_src, _key);

    //FutureKeyManager::getInstance()->returnKey(_key);

    return data;
}

void DirectFutureDataRetriever::releaseData() {}

} // namespace activebsp
