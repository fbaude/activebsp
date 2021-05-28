#include "worker/ActorWorker.h"

#include <mpi.h>

#include "Actor.h"
#include "actormsg/instruction_envelope.h"
#include "actormsg/ActiveObjectRequest.h"
#include "actormsg/CallActorMessage.h"
#include "actormsg/ActiveObjectRequest.h"
#include "ActorCommunicator.h"
#include "IntraActorCommunicator.h"
#include "future/ResultPublisher.h"
#include "management/MasterProxy.h"
#include "SyntaxHelper.h"

#include "ActiveBSPlib.h"

#include "log.h"

namespace activebsp
{

ActorWorker::ActorWorker(ActorBase * obj,
                            const std::shared_ptr<ActorCommunicator> & actorComm,
                            const std::shared_ptr<IntraActorCommunicator> &intraActorComm,
                            const std::shared_ptr<MasterProxy> & masterProxy)
    : _obj(obj), _run(1),
      _actorComm(actorComm),
      _intraActorComm(intraActorComm),
      _masterProxy(masterProxy)
{}

ActorWorker::~ActorWorker() {}

void ActorWorker::register_function(const std::string & name, std::function< std::vector<char> (char *, int, ActorBase *)> f)
{
    _functions[name] = f;
}

bool ActorWorker::is_master()
{
    return _obj->is_master();
}

void ActorWorker::handler()
{
    LOG_TRACE("%s", "Entering handler loop");

    if (is_master())
    {
        master_loop();
    }
    else
    {
        slave_loop();
    }

    destroyActor();
}

void ActorWorker::destroyActor()
{
    if (! is_master())
    {
        _masterProxy->destroyMasterObject();
    }
    else
    {
        _obj->kill_slaves();
    }

    _obj->bsp_sync();

    delete _obj;

    _obj = NULL;
}

void ActorWorker::master_loop()
{
    while (_run || _masterProxy->hasRequest())
    {
        int src, key;

        ActiveObjectRequest req = _masterProxy->getNextRequest();

        src = req.getSource();

        LOG_TRACE("Handling request from P%d", src);

        if (src == -1)
        {
            _run = 0;

            LOG_TRACE("%s", "Received exit flag, exiting master loop");

            break;
        }

        std::shared_ptr<CallActorMessage> callActorMessage = req.getCallActorMessage();

        std::string name = callActorMessage->getFunctionName();
        char * serialBuf = callActorMessage->getArgsPtr();
        int serialSize = callActorMessage->getArgsSize();
        key = callActorMessage->getFutureKey();

        std::vector<char> serial;

        auto it = _functions.find(name);
        if (it != _functions.end())
        {
            LOG_TRACE("Handling request of future key %d", key);

            START_MEASURE(1);

            _obj->setRequestId(key);
            serial = _functions[name](serialBuf, serialSize, _obj);

            END_MEASURE(1);
            LOG_MEASURE(1, "calling function and serializing result");
        }
        else
        {
            std::cerr << "Could not find function with name " << name << std::endl;
            serial = {'e','r','r','o','r'};
        }

        LOG_TRACE("Finished function call, reporting result of key %d", key);

        START_MEASURE(2);

        _masterProxy->reportResult(req, serial);

        END_MEASURE(2);
        LOG_MEASURE(2, "reporting serialized result");
    }

}

void ActorWorker::slave_loop()
{
    while (1)
    {
        int function_id = _intraActorComm->receiveSPMDFunctionId();

        if (function_id == 0)
        {
            LOG_TRACE("%s", "Worker received exit function ID, exiting worker loop");

            break;
        }
        else
        {
            LOG_TRACE("Worker executing parallel function of id %d", function_id);

            _obj->call_spmd(function_id - 1);
        }

        LOG_TRACE("%s", "Worker exiting parallel function");

    }

}

} // namespace activebsp
