#ifndef __GET_FUTURE_DATA_MESSAGE_H__
#define __GET_FUTURE_DATA_MESSAGE_H__

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class GetFutureDataMessage : public ActorMessage
{
public:
    GetFutureDataMessage(int src, char * buf, int buf_size);
    GetFutureDataMessage(int future_key);

    virtual ~GetFutureDataMessage();

    int getFutureKey();
};

} // namespace activebsp

#endif // __GET_FUTURE_DATA_MESSAGE_H__
