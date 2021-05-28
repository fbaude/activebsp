#ifndef __WORKER_MASTER_PROXY_H__
#define __WORKER_MASTER_PROXY_H__

#include <vector>

#include "actormsg/ActiveObjectRequest.h"

namespace activebsp
{

class MasterProxy
{
public:
    virtual void reportResult(const ActiveObjectRequest & req, const std::vector<char> & data) = 0;
    virtual ActiveObjectRequest getNextRequest() = 0;
    virtual bool hasRequest() = 0;
    virtual void destroyMasterObject() = 0;

    virtual int readStoredResult(char * buf, int key, int offset, int size) = 0;
    virtual int writeStoredResult(char * buf, int key, int size) = 0;

    virtual int getNPendingRequests() = 0;

};

} // namespace activebsp

#endif // __WORKER_MASTER_PROXY_H__
