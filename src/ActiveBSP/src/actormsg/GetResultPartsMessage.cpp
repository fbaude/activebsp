#include "actormsg/GetResultPartsMessage.h"

namespace activebsp
{

GetResultPartsMessage::GetResultPartsMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}


GetResultPartsMessage::GetResultPartsMessage(int forward_to, const std::vector<result_req_part_t> & parts)
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(get_result_parts_envelope_t) + parts.size() * sizeof(result_req_part_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    get_result_parts_envelope_t * env = (get_result_parts_envelope_t *) &instruction_envelope->data;
    env->nparts = parts.size();
    env->forward_to = forward_to;

    instruction_envelope->instruction = INSTRUCTION_GET_RESULT_PARTS;

    memcpy(env->data, &parts[0], parts.size() * sizeof(result_req_part_t));
}


GetResultPartsMessage::~GetResultPartsMessage() {}


int GetResultPartsMessage::getNparts() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    get_result_parts_envelope_t * env = (get_result_parts_envelope_t *) &instruction_envelope->data;

    return env->nparts;
}

int GetResultPartsMessage::getForwardTo() const
{
    return getContentEnvelope<get_result_parts_envelope_t>()->forward_to;
}

const result_req_part_t * GetResultPartsMessage::getPart(int i) const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    get_result_parts_envelope_t * env = (get_result_parts_envelope_t *) &instruction_envelope->data;

    result_req_part_t * first = (result_req_part_t *) &env->data[0];

    return first + i;
}

} // namespace activebsp
