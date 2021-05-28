#include "actormsg/CallActorMessage.h"

#include <string.h>

#include "actormsg/instruction_envelope.h"

namespace activebsp
{

CallActorMessage::CallActorMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

CallActorMessage::CallActorMessage(const std::string & function_name, const std::vector<char> &args, int future_key)
{
    int name_size = function_name.size() + 1,
        args_size = args.size();

    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(call_actor_envelope_t) +  name_size + args_size);

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_CALL_ACTOR;

    call_actor_envelope->future_key = future_key;
    call_actor_envelope->args_size  = args_size;

    call_actor_envelope->name_offset = 0;
    call_actor_envelope->args_offset = call_actor_envelope->name_offset + name_size;

    memcpy(ENVELOPE_OFFSET(call_actor_envelope, name_offset), function_name.c_str(), name_size);
    memcpy(ENVELOPE_OFFSET(call_actor_envelope, args_offset), &args[0], args_size);
}

CallActorMessage::~CallActorMessage() {}


std::string CallActorMessage::getFunctionName()
{
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) getEnvelopeContent();

    return std::string (ENVELOPE_OFFSET(call_actor_envelope, name_offset));

}

std::vector<char> CallActorMessage::getArgs()
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) &instruction_envelope->data;

    std::vector<char> ret;
    ret.resize(call_actor_envelope->args_size);
    memcpy(&ret[0], ENVELOPE_OFFSET(call_actor_envelope, args_offset), call_actor_envelope->args_size);

    return ret;
}

char * CallActorMessage::getArgsPtr()
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) &instruction_envelope->data;

    return ENVELOPE_OFFSET(call_actor_envelope, args_offset);
}

int CallActorMessage::getArgsSize()
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) &instruction_envelope->data;

    return call_actor_envelope->args_size;
}

int CallActorMessage::getFutureKey()
{
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) getEnvelopeContent();

    return call_actor_envelope->future_key;
}

} // namespace activebsp
