#include "activebsp.h"

#include <map>
#include <string>
#include <iostream>
#include <functional>

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "management/ActorServer.h"
#include "management/ThreadedMPIActorServerFactory.h"
#include "mpi/MPIWrapper.h"
#include "mpi/MPIActorCommunicator.h"
#include "mpi/MPIIntraActorCommunicatorFactory.h"
#include "SyntaxHelper.h"
#include "actormsg/StopActorProcessMessage.h"
#include "actormsg/CreateActorMessage.h"
#include "actormsg/ReleaseResultPartMessage.h"

namespace activebsp
{

void activebsp_init()
{
    activebsp_init(NULL, NULL);
}

void activebsp_init(int * argc, char ** argv[])
{
    ThreadedMPIActorServerFactory fact;

    std::shared_ptr<MPIWrapper> mpi = fact.createMPIWrapper();

    mpi->init(argc, argv);

    std::shared_ptr<ActorCommunicator> comm = fact.createActorCommunicator(mpi);
    std::shared_ptr<IntraActorCommunicatorFactory> intraActorCommFact = std::make_shared<MPIIntraActorCommunicatorFactory>(mpi);

    SyntaxHelper::createInstance(comm, intraActorCommFact);

    if (mpi->getRank() != 0)
    {
        ActorRegistry * registry = ActorRegistry::getInstance();
        std::shared_ptr<IntraActorCommunicatorFactory> intraActorCommFact = fact.createIntraActorCommunicatorFactory(mpi);
        std::shared_ptr<ActorWorkerFactory> ActorWorkerFactory = fact.createActorWorkerFactory(mpi, comm, intraActorCommFact, registry);

        std::shared_ptr<ActorServer> actorServer = fact.createActorServer(comm, ActorWorkerFactory);

        actorServer->run();

        LOG_TRACE("%s", "Entering safety barrier before finalizing MPI");
        MPI_Barrier(MPI_COMM_WORLD);

        LOG_TRACE("%s", "Finalizing MPI");

        mpi->finalize();
        exit(0);
    }
}

void activebsp_finalize()
{
    LOG_TRACE("%s", "Finalizing, asking all processes to stop");

    StopActorProcessMessage msg;
    SyntaxHelper::getInstance()->getComm()->broadcastActorMessage(msg);

    LOG_TRACE("%s", "Entering safety barrier before finalizing MPI");

    MPI_Barrier(MPI_COMM_WORLD);

    LOG_TRACE("%s", "Finalizing MPI");

    MPI_Finalize();
}

int absp_nprocs()
{
    std::shared_ptr<ActorCommunicator> comm = SyntaxHelper::getInstance()->getComm();

    return comm->nprocs();
}

int absp_pid()
{
    std::shared_ptr<ActorCommunicator> comm = SyntaxHelper::getInstance()->getComm();

    return comm->pid();
}

std::vector<char> dv_get_part(const vector_distribution_base &dv, size_t offset, size_t size)
{
    std::vector<char> v(size);

    dv_get_part(dv, offset, &v[0], size);

    return v;
}

void dv_get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size)
{
    std::shared_ptr<ActorCommunicator> comm = SyntaxHelper::getInstance()->getComm();

    comm->dv_get_part(dv, offset, out_buf, size);
}

void dv_release(const vector_distribution_base & dv)
{
    std::shared_ptr<ActorCommunicator> comm = SyntaxHelper::getInstance()->getComm();

    comm->dv_release(dv);
}

template <class T>
Proxy<T> createActiveObject (const std::vector <int> & pids)
{
    // Error
    std::cerr << "entered wrong createActiveObject" << std::endl;
    exit(1);
}

} // namespace activebsp
