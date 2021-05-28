#include "actormsg/ActiveObjectRequest.h"

#include <string.h>

#include "actormsg/instruction_envelope.h"

namespace activebsp
{
ActiveObjectRequest::ActiveObjectRequest(const std::shared_ptr<CallActorMessage> &callActorMessage, int sender, int localkey)
    : _sender(sender), _localkey(localkey), _callActorMessage(callActorMessage) {}

ActiveObjectRequest::ActiveObjectRequest(const ActiveObjectRequest & other)
    : _sender(other._sender),
      _localkey(other._localkey),
      _callActorMessage(other._callActorMessage)
       {}

ActiveObjectRequest & ActiveObjectRequest::operator = (const ActiveObjectRequest & other)
{
    _sender = other._sender;
    _callActorMessage = other._callActorMessage;
    _localkey = other._localkey;

    return *this;
}

int ActiveObjectRequest::getSource() const { return _sender; }
int ActiveObjectRequest::getKey() const { return _localkey; }


std::shared_ptr<CallActorMessage> ActiveObjectRequest::getCallActorMessage() const
{
    return _callActorMessage;
}


} // namespace activebsp
