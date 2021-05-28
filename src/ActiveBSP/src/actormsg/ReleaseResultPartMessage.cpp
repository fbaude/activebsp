#include "actormsg/ReleaseResultPartMessage.h"

namespace activebsp
{

ReleaseResultPartMessage::ReleaseResultPartMessage(int src, char * buf, int buf_size)
    : ActorMessage(src, buf, buf_size) {}

ReleaseResultPartMessage::ReleaseResultPartMessage(int resid)
{
    allocateOwnBuffer(sizeof(instruction_envelope_t) + sizeof(get_future_data_envelope_t));

    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;
    release_result_part_envelope_t * env = (release_result_part_envelope_t *) &instruction_envelope->data;

    instruction_envelope->instruction = INSTRUCTION_RELEASE_RESULT;

    env->resid = resid;
}

ReleaseResultPartMessage::~ReleaseResultPartMessage() {}

int ReleaseResultPartMessage::getResId()
{
    release_result_part_envelope_t * env = (release_result_part_envelope_t *) getEnvelopeContent();

    return env->resid;
}

} // namespace activebsp
