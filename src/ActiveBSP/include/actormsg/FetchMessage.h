#ifndef FETCHMESSAGE_H
#define FETCHMESSAGE_H

#include "actormsg/ActorMessage.h"
#include "Future.h"

namespace activebsp
{

typedef struct
{
    size_t dv_size;
    int req_future_key;
    int res_future_key;
    int npids;
    char data[];

} fetch_envelope_t;

class FetchMessage : public activebsp::ActorMessage
{
public:
    FetchMessage(int src, char * buf, int buf_size);
    FetchMessage(const int * pids, int npids, int req_future_key, int res_future_key, size_t dv_size);

    FetchMessage(const FetchMessage & other);

    virtual ~FetchMessage();

    int getNpids() const;
    int * getPids() const;
    int * getKeys() const;
    int getReqFutureKey() const;
    int getResFutureKey() const;
    size_t getDvSize() const;
};

} // namespace activebsp


#endif // FETCHMESSAGE_H
