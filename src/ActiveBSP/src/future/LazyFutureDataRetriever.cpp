#include "future/LazyFutureDataRetriever.h"

#include "future/FutureKeyManager.h"
#include "actormsg/ReleaseFutureMessage.h"

// Needed for exporting the class
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

namespace activebsp
{

LazyFutureDataRetriever::LazyFutureDataRetriever(const std::shared_ptr<ActorCommunicator> & comm, int pid, int key)
    : _comm(comm), _key(key), _pid(pid) {}

LazyFutureDataRetriever::LazyFutureDataRetriever()
    : _comm(), _key(-1), _pid(-1) {}


LazyFutureDataRetriever::~LazyFutureDataRetriever() {}

std::shared_ptr<std::vector<char>> LazyFutureDataRetriever::retrieveData()
{
    return _comm->getFutureData(_pid, _key);;
}

void LazyFutureDataRetriever::releaseData()
{
    _comm->sendActorMessage(_pid, ReleaseFutureMessage(_key));
}

int LazyFutureDataRetriever::getFuturePid() const
{
    return _pid;
}

int LazyFutureDataRetriever::getFutureKey() const
{
    return _key;
}

} // namespace activebsp

BOOST_CLASS_EXPORT(activebsp::LazyFutureDataRetriever)
