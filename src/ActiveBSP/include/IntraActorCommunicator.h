#ifndef __INTRA_ACTOR_COMMUNICATOR_H__
#define __INTRA_ACTOR_COMMUNICATOR_H__

#include "vector_distribution.h"

namespace activebsp
{

class IntraActorCommunicator
{
public:
    virtual ~IntraActorCommunicator() {}

    virtual int pid()    = 0;
    virtual int nprocs() = 0;

    virtual int receiveSPMDFunctionId() = 0;
    virtual void sendSPMDFunctionID(int functionId) = 0;

    virtual void gatherKeys(int * recvbuf, int key) = 0;
    virtual void broadcastVd(vector_distribution_base & vd) = 0;

};

} // namespace activebsp

#endif // __INTRA_ACTOR_COMMUNICATOR_H__
