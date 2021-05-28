#ifndef __RELEASE_RESULT_PART_MESSAGE_H__
#define __RELEASE_RESULT_PART_MESSAGE_H__

#include "ActorMessage.h"

namespace activebsp
{

class ReleaseResultPartMessage : public activebsp::ActorMessage
{

public:
    ReleaseResultPartMessage(int src, char * buf, int buf_size);
    ReleaseResultPartMessage(int resid);

    virtual ~ReleaseResultPartMessage();

    int getResId();

};

} // namespace activebsp

#endif // __RELEASE_RESULT_PART_MESSAGE_H__
