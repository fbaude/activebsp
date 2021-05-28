#ifndef __CALL_ACTOR_MESSAGE_HPP__
#define CALLACTORMESSAGE_HPP

#include "CallActorMessage.h"

#include "serialization/serialization.h"

namespace activebsp
{

template <class ... Args>
CallActorMessage::CallActorMessage(char ** buf, size_t * buf_size, const std::string & function_name, int future_key, const Args &... args)
{
    int name_size = function_name.size() + 1,
        args_size = predictSerializationSize(args...);

    _buf_size = sizeof(instruction_envelope_t) + sizeof(call_actor_envelope_t) +  name_size + args_size;

    if (*buf_size < size_t(_buf_size))
    {
        if (*buf_size > 0)
        {
            delete[] *buf;
        }

        *buf = new char[_buf_size];
        *buf_size = _buf_size;
    }

    _buf = *buf;

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_CALL_ACTOR;

    call_actor_envelope->future_key = future_key;
    call_actor_envelope->args_size  = args_size;

    call_actor_envelope->name_offset = 0;
    call_actor_envelope->args_offset = call_actor_envelope->name_offset + name_size;

    memcpy(ENVELOPE_OFFSET(call_actor_envelope, name_offset), function_name.c_str(), name_size);
    serialize_all_to_buf(ENVELOPE_OFFSET(call_actor_envelope, args_offset), args_size, args...);
}

template <class ... Args>
CallActorMessage::CallActorMessage(const std::string & function_name, int future_key, const Args &... args)

{
    int name_size = function_name.size() + 1,
        args_size = predictSerializationSize(args...);

    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(call_actor_envelope_t) +  name_size + args_size);

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    call_actor_envelope_t * call_actor_envelope = (call_actor_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_CALL_ACTOR;

    call_actor_envelope->future_key = future_key;
    call_actor_envelope->args_size  = args_size;

    call_actor_envelope->name_offset = 0;
    call_actor_envelope->args_offset = call_actor_envelope->name_offset + name_size;

    memcpy(ENVELOPE_OFFSET(call_actor_envelope, name_offset), function_name.c_str(), name_size);
    serialize_all_to_buf(ENVELOPE_OFFSET(call_actor_envelope, args_offset), args_size, args...);
}

} // namespace activebsp

#endif // CALLACTORMESSAGE_HPP
