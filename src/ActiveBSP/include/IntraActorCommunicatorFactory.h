#ifndef __INTRA_ACTOR_COMMUNICATOR_FACTORY_H__
#define __INTRA_ACTOR_COMMUNICATOR_FACTORY_H__

#include <memory>
#include <vector>

#include "IntraActorCommunicator.h"

namespace activebsp
{

class IntraActorCommunicatorFactory
{
public:
    virtual ~IntraActorCommunicatorFactory() {}

    virtual std::shared_ptr<IntraActorCommunicator> createIntraActorCommunicator(const std::vector<int> & pids) = 0;
};

} // namespace activebsp

#endif // __INTRA_ACTOR_COMMUNICATOR_FACTORY_H__
