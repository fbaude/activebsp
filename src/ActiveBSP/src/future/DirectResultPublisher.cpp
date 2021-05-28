#include "future/DirectResultPublisher.h"



namespace activebsp
{

DirectResultPublisher::DirectResultPublisher(const std::shared_ptr<ActorCommunicator> & comm)
    : _comm(comm) {}

void DirectResultPublisher::publishResult(const ActiveObjectRequest & req, const std::vector<char> & data)
{
    _comm->sendKeyedFutureValue(req.getSource(), data, req.getSource());

}

} // namespace activebsp
