#include "SyntaxHelper.h"

namespace activebsp
{

SyntaxHelper * SyntaxHelper::_instance = NULL;

void SyntaxHelper::createInstance(std::shared_ptr<ActorCommunicator> comm, std::shared_ptr<IntraActorCommunicatorFactory> intraActorCommunicatorFactory)
{
    _instance = new SyntaxHelper(comm, intraActorCommunicatorFactory);
}

SyntaxHelper::SyntaxHelper(std::shared_ptr <ActorCommunicator> comm, std::shared_ptr<IntraActorCommunicatorFactory> intraActorCommunicatorFactory)
    : _comm(comm), _intraActorCommFact(intraActorCommunicatorFactory) {}

SyntaxHelper * SyntaxHelper::getInstance()
{
    return _instance;
}

std::shared_ptr <ActorCommunicator> SyntaxHelper::getComm()
{
    return _comm;
}

void SyntaxHelper::setIntraActorComm(const std::shared_ptr<IntraActorCommunicator> & intraActorComm)
{
    _intraActorComm = intraActorComm;
}

std::shared_ptr<IntraActorCommunicator> SyntaxHelper::getIntraActorComm()
{
    return _intraActorComm;
}

void SyntaxHelper::setActorBSPlib(BSPlib *bsplib)
{
    _bsplib = bsplib;
}

BSPlib * SyntaxHelper::getActorBSPlib()
{
    return _bsplib;
}


} // namespace activebsp
