#include "actormsg/DistributeVectorMessage.h"

namespace activebsp
{

DistributeVectorMessage::DistributeVectorMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

DistributeVectorMessage::DistributeVectorMessage(distr_type_t type, size_t n, size_t elem_size)
    : ActorMessage()
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof (distribute_vector_envelope_t) + sizeof (block_distr_args_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    distribute_vector_envelope_t * env = (distribute_vector_envelope_t *) &instruction_envelope->data;

    // For now fixed distr type
    instruction_envelope->instruction = INSTRUCTION_DISTRIBUTE_VECTOR;
    env->distr = type;

    block_distr_args_t * args = (block_distr_args_t *) &env->data;
    args->n = n;
    args->elem_size = elem_size;
}

DistributeVectorMessage::~DistributeVectorMessage() {}

distr_type_t DistributeVectorMessage::getDistrType() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    distribute_vector_envelope_t * env = (distribute_vector_envelope_t *) &instruction_envelope->data;

     return env->distr;
}

const void * DistributeVectorMessage::getDistrArgs() const
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    distribute_vector_envelope_t * env = (distribute_vector_envelope_t *) &instruction_envelope->data;
    //block_distr_args_t * args = (block_distr_args_t *) &env->data;

    return &env->data;
}

} // namespace activebsp
