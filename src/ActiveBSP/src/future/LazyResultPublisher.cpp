#include "future/LazyResultPublisher.h"

namespace activebsp
{

LazyResultPublisher::LazyResultPublisher(const std::shared_ptr<ActorCommunicator> & comm)
    : _comm(comm) {}

void LazyResultPublisher::publishResult(const ActiveObjectRequest & req, const std::vector<char> & data)
{
    _comm->registerFutureData(req.getKey(), data);
}

} // namespace activebsp
