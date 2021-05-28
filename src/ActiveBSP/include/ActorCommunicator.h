#ifndef __ACTOR_COMMUNICATOR_H__
#define __ACTOR_COMMUNICATOR_H__

#include <memory>
#include <string>
#include <vector>

#include "actormsg/ActorMessage.h"

namespace activebsp
{

class MasterProxy;

class ActorCommunicator
{
public:
    virtual ~ActorCommunicator() {}

    virtual int askCreateActor(const std::vector<int> & pids, const std::string & name) = 0;

    virtual std::shared_ptr<ActorMessage> receiveActorMessage() = 0;
    virtual void sendActorMessage(int dst, const ActorMessage & msg) = 0;
    virtual void broadcastActorMessage(const ActorMessage & msg) = 0;
    virtual void sendResponseCode(int dst, int responseCode) = 0;
    virtual int receiveResponseCode(int src) = 0;

    virtual void sendKeyedFutureValue(int dst, const std::vector<char> &, int key) = 0;
    virtual std::shared_ptr<std::vector<char>> receiveKeyedFutureValue(int src, int key) = 0;

    virtual void registerFutureData(int key, const std::vector<char> &data) = 0;
    virtual std::shared_ptr<std::vector<char>> getFutureData(int pid, int key) = 0;
    virtual void sendFutureData(int pid, const std::vector<char> & data) = 0;

    virtual void sendResultPart(int pid, const void * src, int size) = 0;
    virtual void receiveResultPart(int pid, void * dst, int size) = 0;

    virtual void sendDistrVector(int pid, const vector_distribution_base & dv) = 0;
    virtual vector_distribution_base receiveDistrVector(int pid) = 0;

    virtual void dv_get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size) = 0;
    virtual void dv_release(const vector_distribution_base & dv) = 0;

    virtual int store_result(int resid, const char * buf, size_t size) = 0;
    
    virtual vector_distribution_base block_distribute(int head, const char * v, size_t n, size_t elem_size) = 0;

    virtual int pid()    const = 0;
    virtual int nprocs() const = 0;

    virtual void setMasterProxy(const std::shared_ptr<MasterProxy> & MasterProxy) = 0;
    virtual void giveBufferOwnershipTo(ActorMessage & msg) = 0;

};

} // namespace activebsp

#endif // __ACTOR_COMMUNICATOR_H__
