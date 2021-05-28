#include "actormsg/ActorMessage.h"

#include <cstddef>

#include "log.h"
#include "unused.h"

namespace activebsp
{

ActorMessage::ActorMessage()
    : _buf(NULL), _buf_size(0), _src(-1), _qowned(false) {}

ActorMessage::ActorMessage(int src, char * buf, int buf_size)
    : _buf(buf), _buf_size(buf_size), _src(src), _qowned(false) {}

ActorMessage::ActorMessage(const ActorMessage & other)
    : _buf(NULL), _buf_size(0), _src(other._src), _qowned(false)
{
    allocateOwnBuffer(other._buf_size);
    memcpy(_buf, other._buf, other._buf_size);
}

ActorMessage::~ActorMessage()
{
    freeOwnedBuffer();
}

// Deprecated
char *ActorMessage::getData()   const { return _buf;      }
int ActorMessage::getDataSize() const { return _buf_size; }

char * ActorMessage::getBuf()     const { return _buf;      }
size_t ActorMessage::getBufSize() const { return _buf_size; }

int ActorMessage::getSrc() const { return _src; }

void ActorMessage::giveBufferOwnership(char * buf, size_t buf_size)
{
    UNUSED(buf_size); // Would be used in more elaborate implementation
    assert(buf == _buf);

    _qowned = true;
}

void * ActorMessage::getEnvelopeContent()
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) _buf;

    return (void *) &instruction_envelope->data;
}

void ActorMessage::allocateBuffer(size_t size)
{
    LOG_TRACE("Allocating new internal buffer of size %zu", size);

    _buf = new char[size];
    _buf_size = size;
}

void ActorMessage::allocateOwnBuffer(size_t size)
{
    allocateBuffer(size);
    _qowned = true;
}

void ActorMessage::enforceBufferSize(size_t size)
{
    LOG_TRACE("Enforcing buffer size if %zu", size);
    if (size_t(_buf_size) < size)
    {
        LOG_TRACE("%s", "Requiring reallocation");
        freeBuffer();
        allocateBuffer(size);
    }
    else
    {
        LOG_TRACE("already has %d bytes, enough for %zu required", _buf_size, size);
    }
}

void ActorMessage::freeBuffer()
{
    LOG_TRACE("%s", "Freeing buffer");

    if (_buf != NULL)
    {
        delete [] _buf;
        _buf = NULL;
        _buf_size = 0;
    }
    else
    {
        LOG_TRACE("%s", "Did not require freeing buffer as it is NULL");
    }
}

void ActorMessage::freeOwnedBuffer()
{
    if (_qowned)
    {
        freeBuffer();
    }
}

instruction_t ActorMessage::getInstruction() const
{
    return getInstructionEnvelope()->instruction;
}

instruction_envelope_t * ActorMessage::getInstructionEnvelope() const
{
    return (instruction_envelope_t *) _buf;
}



} // namespace activebsp
