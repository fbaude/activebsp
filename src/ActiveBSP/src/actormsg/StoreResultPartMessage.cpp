#include "actormsg/StoreResultPartMessage.h"

namespace activebsp
{

StoreResultPartMessage::StoreResultPartMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

void StoreResultPartMessage::build(int resid, const char * data, size_t offset, size_t size, size_t full_size)
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    store_result_part_envelope_t * env = (store_result_part_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_STORE_RESULT_PART;

    env->resid      = resid;
    env->data_size  = size;
    env->full_data_size = full_size;
    env->data_offset = offset;

    memcpy(env->data, data, size);
}


StoreResultPartMessage::StoreResultPartMessage(int resid, const char * data, size_t offset, size_t size, size_t full_size)
{
    size_t msg_size = sizeof(instruction_envelope_t) + sizeof(store_result_part_envelope_t) + size;

    allocateOwnBuffer(msg_size);

    build(resid, data, offset, size, full_size);
}


StoreResultPartMessage::StoreResultPartMessage(char **buf, size_t *buf_size, int resid, const char * data, size_t offset, size_t size, size_t full_size)
{
    size_t msg_size = sizeof(instruction_envelope_t) + sizeof(store_result_part_envelope_t) + size;

    enforceBufferSize(msg_size);
    *buf      = getBuf();
    *buf_size = getBufSize();

    _qowned = false;

    build(resid, data, offset, size, full_size);
}


StoreResultPartMessage::~StoreResultPartMessage() {}

int StoreResultPartMessage::getResId() const
{
    return getContentEnvelope<store_result_part_envelope_t>()->resid;
}

char * StoreResultPartMessage::getData() const
{
    return getContentEnvelope<store_result_part_envelope_t>()->data;
}

size_t StoreResultPartMessage::getDataSize() const
{
    return getContentEnvelope<store_result_part_envelope_t>()->data_size;
}

size_t StoreResultPartMessage::getOffset() const
{
    return getContentEnvelope<store_result_part_envelope_t>()->data_offset;
}


} // namespace activebsp
