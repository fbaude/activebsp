#ifndef __ACTOR_MESSAGE_H__
#define __ACTOR_MESSAGE_H__

#include <memory>

#include "instruction_envelope.h"

namespace activebsp
{

class ActorMessage
{
protected:
    char * _buf;
    int _buf_size;
    int _src;
    bool _qowned;

    void * getEnvelopeContent();
    void allocateBuffer(size_t size);
    void allocateOwnBuffer(size_t size);
    void enforceBufferSize(size_t size);
    void freeOwnedBuffer();
    void freeBuffer();

public:
    ActorMessage();
    ActorMessage(int src, char * buf, int buf_size);
    ActorMessage(const ActorMessage & other);

    ActorMessage & operator = (const ActorMessage & other) = delete;

    virtual ~ActorMessage();

    instruction_t getInstruction() const;
    instruction_envelope_t * getInstructionEnvelope() const;

    template <class T>
    T * getContentEnvelope() const;

    char * getData() const;
    int getDataSize() const;

    char * getBuf() const;
    size_t getBufSize() const;

    int getSrc() const;
    void giveBufferOwnership(char * buf, size_t buf_size);
};

} // namespace activebsp

#include "actormsg/ActorMessage.hpp"

#endif // __ACTOR_MESSAGE_H__
