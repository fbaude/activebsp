#ifndef __LAZY_RESULT_PUBLISHER_H__
#define __LAZY_RESULT_PUBLISHER_H__

#include <memory>

#include "ResultPublisher.h"
#include "ActorCommunicator.h"

namespace activebsp
{

class LazyResultPublisher : public ResultPublisher
{
private:
    std::shared_ptr<ActorCommunicator> _comm;

public:
    LazyResultPublisher(const std::shared_ptr<ActorCommunicator> & comm);

    virtual void publishResult(const ActiveObjectRequest &, const std::vector<char> &);

};

} // namespace activebsp

#endif // __LAZY_RESULT_PUBLISHER_H__
