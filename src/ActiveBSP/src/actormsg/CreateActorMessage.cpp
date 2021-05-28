#include "actormsg/CreateActorMessage.h"

#include <stdlib.h>
#include <string.h>

#include "actormsg/instruction_envelope.h"

namespace activebsp
{

CreateActorMessage::CreateActorMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

CreateActorMessage::CreateActorMessage(const std::string & actorName, const std::vector<int> & pids)
{
    int name_size = (actorName.size() + 1) * sizeof(char);
    int pids_size = pids.size() * sizeof(int);

    _buf_size = sizeof(instruction_envelope_t) + sizeof(create_actor_envelope_t)
              + name_size + pids_size;

    _buf = new char[_buf_size];
    _qowned = true;

    instruction_envelope_t  * instruction_envelope  = (instruction_envelope_t *) _buf;
    create_actor_envelope_t * create_actor_envelope = (create_actor_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_CREATE_ACTOR;

    create_actor_envelope->name_offset = 0;
    create_actor_envelope->pids_offset = name_size;
    create_actor_envelope->pids_length = pids.size();

    memcpy(&create_actor_envelope->data[create_actor_envelope->name_offset], actorName.c_str(), name_size);
    memcpy(&create_actor_envelope->data[create_actor_envelope->pids_offset], &pids[0], pids_size);
}

CreateActorMessage::~CreateActorMessage() {}

std::string CreateActorMessage::getActorName()
{
    create_actor_envelope_t * create_actor_envelope = (create_actor_envelope_t *) getEnvelopeContent();

    return std::string(ENVELOPE_OFFSET(create_actor_envelope, name_offset));
}

std::vector<int> CreateActorMessage::getPids()
{
    create_actor_envelope_t * create_actor_envelope = (create_actor_envelope_t *) getEnvelopeContent();

    std::vector <int> pids(create_actor_envelope->pids_length);
    memcpy(&pids[0], &create_actor_envelope->data[create_actor_envelope->pids_offset], create_actor_envelope->pids_length * sizeof(int));

    return pids;
}

} // namespace activebsp
