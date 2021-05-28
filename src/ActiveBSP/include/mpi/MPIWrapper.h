#ifndef __MPI_WRAPPER_H__
#define __MPI_WRAPPER_H__

#include <mpi.h>

#include <vector>

namespace activebsp
{

class MPIWrapper
{
private:


public:
    void init(int * argc, char *** argv);
    void finalize();

    MPI_Comm getWorld() const;
    int getRank(MPI_Comm comm);
    int getSize(MPI_Comm comm);

    int getRank();
    int getSize();

    void getCount(const MPI_Status * status, MPI_Datatype datatype, int * count);

    void send(const void * buf, int size, MPI_Datatype datatype, int dst, int tag, MPI_Comm comm);
    void isend(const void * buf, int size, MPI_Datatype datatype, int dst, int tag, MPI_Comm comm, MPI_Request * request);

    void probe(int src, int tag, MPI_Comm comm, MPI_Status *status);
    void recv(void * buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, MPI_Status * status);
    void irecv(void * buf, int count, MPI_Datatype datatype, int src, int tag, MPI_Comm comm, MPI_Request * request);

    void waitAll(int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[]);

    MPI_Comm createSubset (MPI_Comm comm, const std::vector<int> & pids);

};

} // namespace activebsp

#endif // __MPI_WRAPPER_H__
