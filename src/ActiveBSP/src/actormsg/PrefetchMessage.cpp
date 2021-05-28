#include "actormsg/PrefetchMessage.h"

namespace activebsp
{

PrefetchMessage::PrefetchMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

PrefetchMessage::PrefetchMessage(int future_pid, int future_key, size_t dv_size)
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(prefetch_envelope_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    prefetch_envelope_t * env = (prefetch_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_PREFETCH;

    env->future_pid = future_pid;
    env->future_key = future_key;
    env->dv_size       = dv_size;
}


PrefetchMessage::~PrefetchMessage() {}

int PrefetchMessage::getFuturePid() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    prefetch_envelope_t * env = (prefetch_envelope_t *) &instruction_envelope->data;

    return env->future_pid;
}

int PrefetchMessage::getFutureKey() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    prefetch_envelope_t * env = (prefetch_envelope_t *) &instruction_envelope->data;

    return env->future_key;
}

size_t PrefetchMessage::getDvSize() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    prefetch_envelope_t * env = (prefetch_envelope_t *) &instruction_envelope->data;

    return env->dv_size;
}

} // namespace activebsp
