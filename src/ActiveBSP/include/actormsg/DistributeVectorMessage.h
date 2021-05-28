#ifndef __DISTRIBUTE_VECTOR_MESSAGE_H__
#define __DISTRIBUTE_VECTOR_MESSAGE_H__

#include "actormsg/ActorMessage.h"

namespace activebsp
{


class DistributeVectorMessage : public activebsp::ActorMessage
{
public:
    DistributeVectorMessage(distr_type_t, size_t n, size_t elem_size);
    DistributeVectorMessage(int src, char *, int size);

    virtual ~DistributeVectorMessage();

    distr_type_t getDistrType() const;
    const void * getDistrArgs() const;
};

} // namespace activebsp

#endif // __DISTRIBUTE_VECTOR_MESSAGE_H__
