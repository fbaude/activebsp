#ifndef __STORE_RESULT_PART_MESSAGE_H__
#define __STORE_RESULT_PART_MESSAGE_H__

#include "actormsg/ActorMessage.h"

namespace activebsp
{

typedef struct
{
    int resid;
    size_t data_size;
    size_t data_offset;
    size_t full_data_size;
    char data[];

} store_result_part_envelope_t;

class StoreResultPartMessage : public activebsp::ActorMessage
{
private:
    void build(int resid, const char * data, size_t offset, size_t size, size_t full_size);

public:
    StoreResultPartMessage(int src, char *buf, int buf_size);
    StoreResultPartMessage(int resid, const char * data, size_t offset, size_t size, size_t full_size);
    StoreResultPartMessage(char ** buf, size_t * buf_size, int resid, const char * data, size_t offset, size_t size, size_t full_size);

    virtual ~StoreResultPartMessage();

    int getResId() const;
    char *getData() const;
    size_t getDataSize() const;
    size_t getOffset() const;

};

} // namespace activebsp


#endif // __STORE_RESULT_PART_MESSAGE_H__
