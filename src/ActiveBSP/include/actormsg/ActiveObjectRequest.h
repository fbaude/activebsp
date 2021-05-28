#ifndef __ACTIVE_OBJECT_REQUEST_H__
#define __ACTIVE_OBJECT_REQUEST_H__

#include <string>

#include "actormsg/instruction_envelope.h"
#include "actormsg/CallActorMessage.h"

namespace activebsp
{
    class ActiveObjectRequest
    {
    private:
        int _sender;
        int _localkey;
        std::shared_ptr<CallActorMessage> _callActorMessage;

    public:
        ActiveObjectRequest(const std::shared_ptr<CallActorMessage> & callActorMessage, int sender, int localkey = 0);
        ActiveObjectRequest(const ActiveObjectRequest&);

        ActiveObjectRequest & operator = (const ActiveObjectRequest&);

        int getSource() const;
        int getKey() const;

        std::shared_ptr<CallActorMessage> getCallActorMessage() const;
    };

} // namespace activebs

#endif // __ACTIVE_OBJECT_REQUEST_H__
