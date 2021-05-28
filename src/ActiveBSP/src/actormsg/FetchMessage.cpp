#include "actormsg/FetchMessage.h"

namespace activebsp
{

FetchMessage::FetchMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

FetchMessage::FetchMessage(const int * pids, int npids, int req_future_key, int res_future_key, size_t dv_size)
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(fetch_envelope_t) + npids * 2 * sizeof(int));

    instruction_envelope_t * instruction_envelope = getInstructionEnvelope();
    fetch_envelope_t * env = getContentEnvelope<fetch_envelope_t>();

    instruction_envelope->instruction = INSTRUCTION_FETCH;

    env->npids      = npids;
    env->req_future_key = req_future_key;
    env->res_future_key = res_future_key;
    env->dv_size    = dv_size;

    memcpy(env->data, pids, npids * sizeof(int));
}

FetchMessage::FetchMessage(const FetchMessage & other)
    : ActorMessage(other) {}


FetchMessage::~FetchMessage() {}


int FetchMessage::getReqFutureKey() const
{
    return getContentEnvelope<fetch_envelope_t>()->req_future_key;
}

int FetchMessage::getResFutureKey() const
{
    return getContentEnvelope<fetch_envelope_t>()->res_future_key;
}

size_t FetchMessage::getDvSize() const
{
    return getContentEnvelope<fetch_envelope_t>()->dv_size;
}

int FetchMessage::getNpids() const
{
    return getContentEnvelope<fetch_envelope_t>()->npids;
}

int * FetchMessage::getPids() const
{
    return (int *) getContentEnvelope<fetch_envelope_t>()->data;
}

int * FetchMessage::getKeys() const
{
    return getPids() + getNpids();
}

} // namespace activebsp
