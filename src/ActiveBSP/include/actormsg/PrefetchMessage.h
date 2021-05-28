#ifndef __PREFETCH_MESSAGE_H__
#define __PREFETCH_MESSAGE_H__

#include "actormsg/ActorMessage.h"
#include "Future.h"

namespace activebsp
{

typedef struct
{
    int future_pid;
    int future_key;
    size_t dv_size;

} prefetch_envelope_t;

class PrefetchMessage : public activebsp::ActorMessage
{
public:
    PrefetchMessage(int src, char * buf, int buf_size);
    PrefetchMessage(int future_pid, int future_key, size_t dv_size);

    virtual ~PrefetchMessage();

    int getFuturePid() const;
    int getFutureKey() const;
    size_t getDvSize() const;
};

} // namespace activebsp


#endif // __PREFETCH_MESSAGE_H__
