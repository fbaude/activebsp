#ifndef __RELEASE_FUTURE_MESSAGE_H__
#define __RELEASE_FUTURE_MESSAGE_H__

#include "ActorMessage.h"

namespace activebsp
{

class ReleaseFutureMessage : public activebsp::ActorMessage
{

public:
    ReleaseFutureMessage(int src, char *buf, int buf_size);
    ReleaseFutureMessage(int future_key);

    virtual ~ReleaseFutureMessage();

    int getFutureKey();

};

} // namespace activebsp

#endif // __RELEASE_FUTURE_MESSAGE_H__
