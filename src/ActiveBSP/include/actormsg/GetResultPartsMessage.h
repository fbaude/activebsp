#ifndef __GET_RESULT_PARTS_MESSAGE_H__
#define __GET_RESULT_PARTS_MESSAGE_H__

#include "actormsg/ActorMessage.h"

#include "vector_distribution.h"

namespace activebsp
{

typedef struct
{
    int resid;
    size_t offset;
    size_t size;
    char * dst;

} result_req_part_t;

typedef struct
{
    int nparts;
    int forward_to;
    char data[];

} get_result_parts_envelope_t;

class GetResultPartsMessage : public ActorMessage
{
public:
    GetResultPartsMessage(int src, char *buf, int buf_size);
    GetResultPartsMessage(int forward_to, const std::vector<result_req_part_t> & parts);

    virtual ~GetResultPartsMessage();

    int getNparts() const;
    int getForwardTo() const;
    const result_req_part_t * getPart(int i) const;

};

} // namespace activebsp

#endif // __GET_RESULT_PARTS_MESSAGE_H__
