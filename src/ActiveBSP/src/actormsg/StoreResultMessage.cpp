#include "actormsg/StoreResultMessage.h"

namespace activebsp
{

StoreResultMessage::StoreResultMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

void StoreResultMessage::build(int resid, const char * data, size_t size, bool only_address)
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_envelope_t * env = (store_result_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_STORE_RESULT;

    env->resid      = resid;
    env->data_size  = size;
    env->is_addr    = only_address;

    if (only_address)
    {
        memcpy(env->data, &data, sizeof(const char *));
    }
    else
    {
        memcpy(env->data, data, size);
    }
}


StoreResultMessage::StoreResultMessage(int resid, const char * data, size_t size, bool only_address)
{
    size_t data_size = only_address ? sizeof (const char *) : size;
    size_t msg_size = sizeof(instruction_envelope_t) + sizeof(store_result_envelope_t) + data_size;

    allocateOwnBuffer(msg_size);

    build(resid, data, size, only_address);
}

StoreResultMessage::StoreResultMessage(char **buf, size_t *buf_size, int resid, const char * data, size_t size, bool only_address)
    : ActorMessage(-1, *buf, *buf_size)
{
    size_t data_size = only_address ? sizeof (const char *) : size;
    size_t msg_size = sizeof(instruction_envelope_t) + sizeof(store_result_envelope_t) + data_size;

    enforceBufferSize(msg_size);
    *buf      = getBuf();
    *buf_size = getBufSize();

    _qowned = false;

    build(resid, data, size, only_address);
}

StoreResultMessage::~StoreResultMessage() {}

int StoreResultMessage::getResId() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_envelope_t * env = (store_result_envelope_t *) &instruction_envelope->data;

    return env->resid;
}

char * StoreResultMessage::getData() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_envelope_t * env = (store_result_envelope_t *) &instruction_envelope->data;

    return env->data;
}

size_t StoreResultMessage::getDataSize() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_envelope_t * env = (store_result_envelope_t *) &instruction_envelope->data;

    return env->data_size;
}

bool StoreResultMessage::isOnlyAddr() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_envelope_t * env = (store_result_envelope_t *) &instruction_envelope->data;

    return env->is_addr;
}

const char * StoreResultMessage::getAddrData() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_envelope_t * env = (store_result_envelope_t *) &instruction_envelope->data;

    const char * addr;
    memcpy(&addr, env->data, sizeof(const char *));

    return addr;
}

} // namespace activebsp
