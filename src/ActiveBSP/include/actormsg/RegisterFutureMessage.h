#ifndef __REGISTER_FUTURE_MESSAGE_H__
#define __REGISTER_FUTURE_MESSAGE_H__

#include <vector>

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class RegisterFutureMessage : public activebsp::ActorMessage
{

public:
    RegisterFutureMessage(int src, char *buf, int buf_size);
    RegisterFutureMessage(int future_key, std::vector<char> future_data);

    virtual ~RegisterFutureMessage();

    int getFutureKey();
    std::vector<char> getFutureData();

};

} // namespace activebsp

#endif // __REGISTER_FUTURE_MESSAGE_H__
