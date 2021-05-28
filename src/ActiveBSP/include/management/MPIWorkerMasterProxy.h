#ifndef __MPI_WORKER_MASTER_PROXY_H__
#define __MPI_WORKER_MASTER_PROXY_H__

#include "ActorCommunicator.h"
#include "mpi/MPIWrapper.h"
#include "ResultPublisher.h"
#include "MasterProxy.h"
#include "ActiveObjectRequest.h"

namespace activebsp
{

class MPIWorkerMasterProxy : public MasterProxy
{
private:
    int _master;
    std::shared_ptr<ActorCommunicator> _comm;
    std::shared_ptr<MPIWrapper> _mpi;
    std::shared_ptr<ResultPublisher> _publisher;

public:
    MPIWorkerMasterProxy(int master,
                         const std::shared_ptr<ActorCommunicator> & comm,
                         const std::shared_ptr<MPIWrapper> & _mpi,
                         const std::shared_ptr<ResultPublisher> & publisher);

    virtual void reportResult(const ActiveObjectRequest & req, const std::vector<char> & data);
    virtual ActiveObjectRequest getNextRequest();
    virtual bool hasRequest();

};

} // namespace activebsp

#endif // __MPI_WORKER_MASTER_PROXY_H__
