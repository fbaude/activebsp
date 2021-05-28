#include "mpi/MPIActorCommunicator.h"

#include <memory>
#include <iostream>
#include <list>

#include <mpi.h>

#include "management/MasterProxy.h"
#include "actormsg/ActorMessageFactory.h"
#include "actormsg/CreateActorMessage.h"
#include "actormsg/RegisterFutureMessage.h"
#include "actormsg/GetFutureDataMessage.h"
#include "actormsg/GetResultPartsMessage.h"
#include "actormsg/ReleaseResultPartMessage.h"
#include "actormsg/StoreResultMessage.h"
#include "actormsg/DistributeVectorMessage.h"
#include "log.h"

#define ABSP_MPI_MANAGEMENT_THREAD_TAG 0
#define ABSP_MPI_WORKER_THREAD_TAG 1

#define LOCAL_GET_PART_OPT 1

namespace activebsp
{

MPIActorCommunicator::MPIActorCommunicator(std::shared_ptr<MPIWrapper> mpi)
    : _mpi(mpi), _snd_msg_buf_size(0), _send_buf(NULL), _send_buf_size(0), _recv_buf(NULL), _recv_buf_size(0), _rcv_msg_buf_size(0) {}

MPIActorCommunicator::~MPIActorCommunicator() {}

int MPIActorCommunicator::askCreateActor(const std::vector<int> & pids, const std::string & name)
{
    int head = pids[0];

    CreateActorMessage createActorMessage(name, pids);

    for (std::vector<int>::const_iterator pid_it = pids.begin(); pid_it != pids.end(); ++pid_it)
    {
        sendActorMessage(*pid_it, createActorMessage);
    }

    return receiveResponseCode(head);
}

std::shared_ptr<ActorMessage> MPIActorCommunicator::receiveActorMessage()
{
    MPI_Comm comm = _mpi->getWorld();

    MPI_Status status;

    _mpi->probe(MPI_ANY_SOURCE, ABSP_MPI_MANAGEMENT_THREAD_TAG, comm, &status);

    int source = status.MPI_SOURCE;
    int tag = status.MPI_TAG;
    int buf_size;
    _mpi->getCount(&status, MPI_CHAR, &buf_size);

    LOG_TRACE("Receiving actor message of size %d from %d", buf_size, source);

    if (_recv_buf_size < size_t(buf_size))
    {
        LOG_TRACE("Receive message buffer is too small, resizing from %zu to %zu", _recv_buf_size, size_t(buf_size));

        START_MEASURE(1);

        if (_recv_buf != NULL)
        {
            delete[] _recv_buf;
        }

        _recv_buf_size = buf_size;
        _recv_buf = new char[_recv_buf_size];

        if (_recv_buf == NULL)
        {
            LOG_ERROR("Failed allocating buffer of size %zu", _recv_buf_size);
        }

        END_MEASURE(1);
        LOG_MEASURE(1, "resizing buffer");

    }

    LOG_TRACE("Receiving probed actor message from p%d with tag %d at address %p of buff_size %d", source, tag, _recv_buf, buf_size);

    START_MEASURE(1);
    _mpi->recv(_recv_buf, buf_size, MPI_CHAR, source, tag, comm, MPI_STATUS_IGNORE);
    END_MEASURE(1);
    LOG_MEASURE(1, "Receiving message");

    return ActorMessageFactory().createMessage(source, _recv_buf, buf_size);
}

void MPIActorCommunicator::sendAsyncActorMessage(int dst, const ActorMessage &msg, MPI_Request * req)
{
    _mpi->isend(msg.getData(), msg.getDataSize(), MPI_CHAR, dst, ABSP_MPI_MANAGEMENT_THREAD_TAG, _mpi->getWorld(), req);
}

void MPIActorCommunicator::sendActorMessage(int dst, const ActorMessage & msg)
{
    LOG_TRACE("Sending synchronous message to P%d", dst);
    START_MEASURE(1);

    _mpi->send(msg.getData(), msg.getDataSize(), MPI_CHAR, dst, ABSP_MPI_MANAGEMENT_THREAD_TAG, _mpi->getWorld());

    END_MEASURE(1);
    LOG_MEASURE(1, "Sending message");

    LOG_TRACE("Finished sending synchronous message to P%d", dst);

}

void MPIActorCommunicator::broadcastActorMessage(const ActorMessage & msg)
{
    for (int i = 1; i < _mpi->getSize(); ++i)
    {
        sendActorMessage(i, msg);
    }
}

void MPIActorCommunicator::sendResponseCode(int dst, int responseCode)
{
    _mpi->send(&responseCode, 1, MPI_INT, dst, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld());
}

int MPIActorCommunicator::receiveResponseCode(int src)
{
    int responseCode;
    _mpi->recv(&responseCode, 1, MPI_INT, src, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld(), MPI_STATUS_IGNORE);

    return responseCode;
}

void MPIActorCommunicator::sendKeyedFutureValue(int dst, const std::vector<char> & serial, int key)
{
    _mpi->send(&serial[0], serial.size() + 1, MPI_BYTE, dst, key, _mpi->getWorld());
}

std::shared_ptr<std::vector<char>> MPIActorCommunicator::receiveKeyedFutureValue(int src, int key)
{
    MPI_Status status;
    int data_size;

    _mpi->probe(src, key, _mpi->getWorld(), &status);
    _mpi->getCount(&status, MPI_BYTE, &data_size);

    std::shared_ptr<std::vector<char>> v = std::make_shared<std::vector<char>>(data_size);

    _mpi->recv(&(*v)[0], data_size, MPI_BYTE, src, key, _mpi->getWorld(), MPI_STATUS_IGNORE);

    return v;
}

void MPIActorCommunicator::registerFutureData(int key, const std::vector<char> & data)
{
    RegisterFutureMessage msg(key, data);
    sendActorMessage(_mpi->getRank(), msg);
}


std::shared_ptr<std::vector<char>> MPIActorCommunicator::getFutureData(int pid, int key)
{
    LOG_TRACE("Requesting future of key %d from P%d", key, pid);

    GetFutureDataMessage msg(key);
    sendActorMessage(pid, msg);

    MPI_Status status;
    int data_size;

    _mpi->probe(pid, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld(), &status);
    _mpi->getCount(&status, MPI_BYTE, &data_size);

    LOG_TRACE("Receiving answer future of key %d from %d that has size %d", key, pid, data_size);

    std::shared_ptr<std::vector<char>> v = std::make_shared<std::vector<char>>(data_size);

    _mpi->recv(&(*v)[0], data_size, MPI_BYTE, pid, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld(), MPI_STATUS_IGNORE);

    return v;
}

void MPIActorCommunicator::sendFutureData(int pid, const std::vector<char> & data)
{
    _mpi->send(&data[0], data.size(), MPI_BYTE, pid, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld());
}

void MPIActorCommunicator::sendResultPart(int pid, const void *src, int size)
{
    _mpi->send(src, size, MPI_BYTE, pid, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld());
}


void MPIActorCommunicator::receiveResultPart(int pid, void * dst, int size)
{
    _mpi->recv(dst, size, MPI_BYTE, pid, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld(), MPI_STATUS_IGNORE);
}

void MPIActorCommunicator::sendDistrVector(int pid, const vector_distribution_base & dv)
{
    _mpi->send(dv.getBuf(), dv.getBufSize(), MPI_BYTE, pid, ABSP_MPI_MANAGEMENT_THREAD_TAG, _mpi->getWorld());
}

vector_distribution_base MPIActorCommunicator::receiveDistrVector(int pid)
{
    MPI_Status status;
    int nparts, rcv_count;

    _mpi->probe(pid, ABSP_MPI_MANAGEMENT_THREAD_TAG, _mpi->getWorld(), &status);
    _mpi->getCount(&status, MPI_BYTE, &rcv_count);

    nparts = (rcv_count - vector_distribution_base::parts_offset) / vector_distribution_base::parts_size;
    vector_distribution_base dv(nparts);

    _mpi->recv(dv.getBuf(), rcv_count, MPI_BYTE, pid, ABSP_MPI_MANAGEMENT_THREAD_TAG, _mpi->getWorld(), MPI_STATUS_IGNORE);

    return dv;
}

void MPIActorCommunicator::dv_get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size)
{
    START_MEASURE(1);
    START_MEASURE(2);

    int nparts = dv.nparts();
    size_t req_end = offset + size;

    std::map<int, std::vector<result_req_part_t> > reqs;

    LOG_TRACE("get_part requesting offset %zu and size %zu", offset, size);

    // Deciding on what part to retrieve from which process
    // Looping on every part of the vector distribution and see if we need some of it
    int nreqs = 0;
    for (int ipart = 0; ipart < nparts; ++ipart)
    {
        const vector_distribution_part_t * part = dv.getPart(ipart);

        size_t part_end = part->offset + part->size;

        // If we are not interested in any subpart, see the next part
        // If this dv part finishes before the part we are interested in
        // or if it starts after it, then it not a subset of the part we want
        if (part_end <= offset || part->offset >= req_end) continue;

        LOG_TRACE("selected part on P%d at key %d of offset %zu and size %zu", part->pid, part->resid, part->offset, part->size);

        size_t req_offset = offset >= part->offset ? offset - part->offset : 0;
        size_t req_size = std::min( part->size, req_end - part->offset) - req_offset;

        LOG_TRACE("Decided on subpart on P%d at key %d of offset %zu and size %zu", part->pid, part->resid, req_offset, req_size);

#if LOCAL_GET_PART_OPT
        if (part->pid == pid())
        {
            LOG_TRACE("%s", "Reading part from shared memory");

            START_MEASURE(4);

            _masterProxy->readStoredResult(out_buf + req_offset + part->offset - offset, part->resid, req_offset, req_size);

            END_MEASURE(4);

            LOG_MEASURE(4, "Reading part from shared memory");
            LOG_TRACE("%s", "Finished reading part from shared memory");

            continue;
        }

#endif
        // Storing the request for pid who owns it
        reqs[part->pid].push_back({ part->resid, req_offset,
                                    req_size, out_buf + req_offset + part->offset - offset});

        ++nreqs;
    }

    END_MEASURE(2);
    LOG_MEASURE(2, "computing part requests");

    START_MEASURE(3);

    // Prepare to remember MPI requests

    _reqs_buf.resize(nreqs + reqs.size());
    int ireqs = 0;

    // Send requests and remember both messages and MPI requests

    LOG_TRACE("%s", "Sending part requests");

    std::list<GetResultPartsMessage> msgs;

    for (auto it = reqs.begin(); it != reqs.end(); ++it)
    {
        int s = it->first;

        msgs.emplace_back(pid(), it->second);
        GetResultPartsMessage & msg = msgs.back();
        sendAsyncActorMessage(s, msg, &_reqs_buf[ireqs++]);
    }

    END_MEASURE(3);

    LOG_MEASURE(3, "Sending reqs");

    START_MEASURE(4);

    // Prepare to receive all parts into buffer at right position

    for (auto it = reqs.begin(); it != reqs.end(); ++it)
    {
        int s = it->first;

        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
        {
            _mpi->irecv(it2->dst, it2->size, MPI_BYTE, s, ABSP_MPI_WORKER_THREAD_TAG, _mpi->getWorld(), &_reqs_buf.at(ireqs++));
        }
    }

    // Wait to receive all parts (fulfill all MPI irecv requests)

    LOG_TRACE("%s", "Waiting to receive all subparts");

    _mpi->waitAll(ireqs, &_reqs_buf[0], MPI_STATUSES_IGNORE);

    LOG_TRACE("%s", "Got all subparts");

    END_MEASURE(4);

    LOG_MEASURE(4, "Receiving parts");

    END_MEASURE(1);

    LOG_MEASURE(1, "get_part");
}

vector_distribution_base MPIActorCommunicator::block_distribute(int head, const char * v, size_t n, size_t elem_size)
{
    DistributeVectorMessage msg(DISTR_BLOCK, n, elem_size);
    sendActorMessage(head, msg);

    vector_distribution_base dv = receiveDistrVector(head);
    int nparts = dv.nparts();

    std::list<StoreResultMessage> msgs;
    _reqs_buf.resize(nparts);
    int ireqs = 0;

    for (int i = 0, nparts = dv.nparts(); i < nparts; ++i)
    {
        const vector_distribution_part_t * part = dv.getPart(i);
        msgs.emplace_back(part->resid, v + part->offset, part->size);
        StoreResultMessage & msg = msgs.back();
        sendAsyncActorMessage(part->pid, msg, &_reqs_buf[ireqs++]);
    }

    _mpi->waitAll(ireqs, &_reqs_buf[0], MPI_STATUSES_IGNORE);

    return dv;
}

void MPIActorCommunicator::dv_release(const vector_distribution_base &dv)
{
    int nparts = dv.nparts();
    _reqs_buf.resize(nparts);
    int ireqs = 0;

    for (int i = 0; i < nparts; ++i)
    {
        const vector_distribution_part_t * part = dv.getPart(i);
        ReleaseResultPartMessage msg(part->resid);
        sendAsyncActorMessage(part->pid, msg, &_reqs_buf[ireqs++]);
    }

    _mpi->waitAll(_reqs_buf.size(), &_reqs_buf[0], MPI_STATUSES_IGNORE);
}

int MPIActorCommunicator::store_result(int resid, const char * buf, size_t size)
{
    LOG_TRACE("Storing result of id %d and size %zu", resid, size);

    START_MEASURE(1);

    START_MEASURE(2);
    StoreResultMessage msg(&_send_buf, &_send_buf_size, resid, buf, size);
    END_MEASURE(2);

    START_MEASURE(3);
    sendActorMessage(pid(), msg);
    END_MEASURE(3);

    END_MEASURE(1);

    LOG_MEASURE(2, "Creating StoreResultMessage");
    LOG_MEASURE(3, "Sending StoreResult message");
    LOG_MEASURE(1, "Storing result");

    return resid;
}



int MPIActorCommunicator::pid() const
{
    return _mpi->getRank();
}

int MPIActorCommunicator::nprocs() const
{
    return _mpi->getSize();
}

void MPIActorCommunicator::setMasterProxy(const std::shared_ptr<MasterProxy> & masterProxy)
{
    _masterProxy = masterProxy;
}

void MPIActorCommunicator::giveBufferOwnershipTo(ActorMessage & msg)
{
    msg.giveBufferOwnership(_recv_buf, _recv_buf_size);
    _recv_buf = new char[_recv_buf_size];
    memset(_recv_buf, 0, _recv_buf_size);
}


} // namespace activebsp
