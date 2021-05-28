#include "Proxy.h"

#include <string.h>

#include <mpi.h>

#include "actormsg/StopActorMessage.h"
#include "actormsg/DistributeVectorMessage.h"
#include "actormsg/StoreResultMessage.h"
#include "actormsg/PrefetchMessage.h"

namespace activebsp
{

ProxyBase::ProxyBase(const std::shared_ptr<ActorCommunicator> &comm, int dst)
    : _comm(comm), _dst(dst), _send_buf(NULL), _send_buf_size(0) {}

ProxyBase::ProxyBase()
    : _send_buf(NULL), _send_buf_size(0) {}

ProxyBase::~ProxyBase()
{
    if(_send_buf != NULL)
    {
        delete[] _send_buf;
    }
}


void ProxyBase::setComm(const std::shared_ptr<ActorCommunicator> &comm)
{
    _comm = comm;
}

void ProxyBase::setDst(int dst)
{
    _dst = dst;
}

void ProxyBase::destroyObject()
{
    LOG_TRACE("Asking to destroy object owned by head P%d", _dst);

    StopActorMessage msg;
    _comm->sendActorMessage(_dst, msg);

    int ret = _comm->receiveResponseCode(_dst);

    assert(ret == 0);
}

} // namespace activebsp
