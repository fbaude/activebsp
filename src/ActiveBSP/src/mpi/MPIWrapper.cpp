#include "mpi/MPIWrapper.h"

#include "activebsp_init_config.h"
#include "error_codes.h"

#include "log.h"

namespace activebsp
{

void MPIWrapper::init(int * argc, char *** argv)
{
    int flag, provided, requested = ACTIVEBSP_MPI_THREAD_REQUIRED;
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Initialized(&flag);

    if (flag)
    {
        MPI_Query_thread(&provided);
    }
    else
    {
        MPI_Init_thread(argc, argv, requested, &provided);
    }

    if (requested < provided)
    {
        MPI_Abort(comm, 1);
    }

}

void MPIWrapper::finalize()
{
    LOG_TRACE("%s", "Calling MPI_Finalize");
    MPI_Finalize();
}

MPI_Comm MPIWrapper::getWorld() const
{
    return MPI_COMM_WORLD;
}

int MPIWrapper::getRank(MPI_Comm comm)
{
    LOG_TRACE("%s", "Calling MPI_Comm_rank");
    int rank;
    MPI_Comm_rank(comm, &rank);

    return rank;
}

int MPIWrapper::getSize(MPI_Comm comm)
{
    LOG_TRACE("%s", "Calling MPI_Comm_size");
    int size;
    MPI_Comm_size(comm, &size);

    return size;
}

int MPIWrapper::getRank()
{
    return getRank(getWorld());
}

int MPIWrapper::getSize()
{
    return getSize(getWorld());
}

void MPIWrapper::send(const void * buf, int size, MPI_Datatype datatype, int dst, int tag /* = 0 */, MPI_Comm comm /* = getWorld() */)
{
    LOG_TRACE("%s", "Calling MPI_Send");
    MPI_Send(buf, size, datatype, dst, tag, comm);
}

void MPIWrapper::isend(const void * buf, int size, MPI_Datatype datatype, int dst, int tag, MPI_Comm comm, MPI_Request * request)
{
    LOG_TRACE("%s", "Calling MPI_Isend");
    MPI_Isend(buf, size, datatype, dst, tag, comm, request);
}


void MPIWrapper::probe(int src, int tag, MPI_Comm comm, MPI_Status * status)
{
    LOG_TRACE("%s", "Calling MPI_Probe");
    MPI_Probe(src, tag, comm, status);
}

void MPIWrapper::getCount(const MPI_Status * status, MPI_Datatype datatype, int * count)
{
    LOG_TRACE("%s", "Calling MPI_Get_count");
    MPI_Get_count(status, datatype, count);
}

void MPIWrapper::recv(void * buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, MPI_Status * status)
{
    LOG_TRACE("%s", "Calling MPI_Recv");
    MPI_Recv(buf, count, datatype, src, tag, comm, status);
}

void MPIWrapper::irecv(void * buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, MPI_Request * request)
{
    LOG_TRACE("%s", "Calling MPI_Irecv");
    MPI_Irecv(buf, count, datatype, src, tag, comm, request);
}

void MPIWrapper::waitAll(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[])
{
    LOG_TRACE("%s", "Calling MPI_Waitall");
    MPI_Waitall(count, array_of_requests, array_of_statuses);
}

MPI_Comm MPIWrapper::createSubset (MPI_Comm comm, const std::vector<int> & pids)
{
    MPI_Group comm_group;
    MPI_Comm_group(comm, &comm_group);

    MPI_Group subset_group;
    MPI_Group_incl(comm_group, pids.size(), &pids[0], &subset_group);

    MPI_Comm subset_comm;

    MPI_Comm_create_group(comm, subset_group, 0, &subset_comm);

    MPI_Group_free(&comm_group);
    MPI_Group_free(&subset_group);

    return subset_comm;
}

} // namespace activebsp
