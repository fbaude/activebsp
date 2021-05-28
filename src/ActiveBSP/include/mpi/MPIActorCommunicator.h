#ifndef __MPI_ACTOR_COMMUNICATOR_H__
#define __MPI_ACTOR_COMMUNICATOR_H__

#include "ActorCommunicator.h"
#include "MPIWrapper.h"

#include <mpi.h>

namespace activebsp
{

class MasterProxy;

class MPIActorCommunicator : public ActorCommunicator
{
private:
    std::shared_ptr<MPIWrapper> _mpi;
    std::vector<MPI_Request> _reqs_buf;

    std::shared_ptr<char> _snd_msg_buf;
    size_t _snd_msg_buf_size;
    char * _send_buf;
    size_t _send_buf_size;
    std::shared_ptr<char> _rcv_msg_buf;
    char * _recv_buf;
    size_t _recv_buf_size;
    size_t _rcv_msg_buf_size;
    std::shared_ptr<MasterProxy> _masterProxy;

public:
    MPIActorCommunicator(std::shared_ptr<MPIWrapper> mpi);

    virtual ~MPIActorCommunicator();

    virtual int askCreateActor(const std::vector<int> & pids, const std::string & name);

    virtual std::shared_ptr<ActorMessage> receiveActorMessage();
            void sendAsyncActorMessage(int dst, const ActorMessage &msg, MPI_Request * req);
    virtual void sendActorMessage(int dst, const ActorMessage &msg);
    virtual void broadcastActorMessage(const ActorMessage & msg);

    virtual void sendResponseCode(int dst, int responseCode);
    virtual int receiveResponseCode(int src);

    virtual void sendKeyedFutureValue(int dst, const std::vector<char> & serial, int key);
    virtual std::shared_ptr<std::vector<char>> receiveKeyedFutureValue(int src, int key);

    virtual void registerFutureData(int key, const std::vector<char> &data);
    virtual std::shared_ptr<std::vector<char>> getFutureData(int pid, int key);
    virtual void sendFutureData(int pid, const std::vector<char> & data);

    virtual void sendResultPart(int pid, const void * src, int size);
    virtual void receiveResultPart(int pid, void * dst, int size);

    virtual void sendDistrVector(int pid, const vector_distribution_base & dv);
    virtual vector_distribution_base receiveDistrVector(int pid);

    virtual void dv_get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size);
    virtual void dv_release(const vector_distribution_base & dv);

    virtual int store_result(int resid, const char * buf, size_t size);
    virtual vector_distribution_base block_distribute(int head, const char * v, size_t n, size_t elem_size);

    virtual int pid()    const;
    virtual int nprocs() const;

    virtual void setMasterProxy(const std::shared_ptr<MasterProxy> & MasterProxy);

    virtual void giveBufferOwnershipTo(ActorMessage & msg);

};

} // namespace activebsp

#endif // __MPI_ACTOR_COMMUNICATOR_H__
