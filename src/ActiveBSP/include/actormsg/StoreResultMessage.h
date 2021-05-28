#ifndef __STORE_RESULT_MESSAGE_H__
#define __STORE_RESULT_MESSAGE_H__

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class StoreResultMessage : public activebsp::ActorMessage
{
private:
    void build(int resid, const char * data, size_t size, bool only_address = false);

public:
    StoreResultMessage(int src, char *buf, int buf_size);
    StoreResultMessage(int resid, const char * data, size_t size, bool only_address = false);
    StoreResultMessage(char ** buf, size_t * buf_size, int resid, const char * data, size_t size, bool only_address = false);

    virtual ~StoreResultMessage();

    int getResId() const;
    char *getData() const;
    size_t getDataSize() const;

    bool isOnlyAddr() const;
    const char *getAddrData() const;

};

} // namespace activebsp

#endif // __STORE_RESULT_MESSAGE_H__
