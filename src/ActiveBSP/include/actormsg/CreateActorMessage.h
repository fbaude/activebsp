#ifndef __CREATE_ACTOR_MESSAGE_H__
#define __CREATE_ACTOR_MESSAGE_H__

#include <string>
#include <vector>

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class CreateActorMessage : public activebsp::ActorMessage
{

public:
    CreateActorMessage(int src, char *buf, int buf_size);
    CreateActorMessage(const std::string & actorName, const std::vector<int> & pids);

    virtual ~CreateActorMessage();

    std::string getActorName();
    std::vector<int> getPids();

};

} // namespace activebsp

#endif // CREATEACTORMESSAGE_H
