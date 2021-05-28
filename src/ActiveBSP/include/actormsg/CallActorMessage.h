#ifndef __CALL_ACTOR_MESSAGE_H__
#define __CALL_ACTOR_MESSAGE_H__

#include <string>
#include <vector>

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class CallActorMessage : public activebsp::ActorMessage
{
public:
    CallActorMessage(int src, char * buf, int buf_size);
    CallActorMessage(const std::string & function_name, const std::vector<char> & args, int future_key);

    template <class ... Args>
    CallActorMessage(char ** buf, size_t * buf_size, const std::string & function_name, int future_key, const Args &... args);

    template <class ... Args>
    CallActorMessage(const std::string & function_name, int future_key, const Args &... args);

    virtual ~CallActorMessage();

    std::string getFunctionName();
    std::vector<char> getArgs();
    int getFutureKey();

    char * getArgsPtr();
    int getArgsSize();

};

} // namespace activebsp

#include "CallActorMessage.hpp"

#endif // __CALL_ACTOR_MESSAGE_H__
