#ifndef __DIRECT_RESULT_PUBLISHER_H__
#define __DIRECT_RESULT_PUBLISHER_H__

#include <memory>

#include "ResultPublisher.h"
#include "ActorCommunicator.h"

namespace activebsp
{

class DirectResultPublisher : public ResultPublisher
{
private:
    std::shared_ptr<ActorCommunicator> _comm;

public:
    DirectResultPublisher(const std::shared_ptr<ActorCommunicator> & comm);

    virtual void publishResult(const ActiveObjectRequest &, const std::vector<char> &);

};

} // namespace activebsp

#endif // __DIRECT_RESULT_PUBLISHER_H__
