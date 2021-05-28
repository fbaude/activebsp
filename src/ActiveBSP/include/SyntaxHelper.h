#ifndef __SYNTAX_HELPER_H__
#define __SYNTAX_HELPER_H__

#include <memory>

#include "ActorCommunicator.h"
#include "ActorRegistry.h"
#include "IntraActorCommunicatorFactory.h"

namespace activebsp
{

// A nasty global variable that should hopefully only be used because of user syntax constraint

class SyntaxHelper
{
private:
    static SyntaxHelper * _instance;

    std::shared_ptr <ActorCommunicator> _comm;
    std::shared_ptr <IntraActorCommunicatorFactory> _intraActorCommFact;
    std::shared_ptr <IntraActorCommunicator> _intraActorComm;
    BSPlib * _bsplib;

    SyntaxHelper(std::shared_ptr <ActorCommunicator> comm, std::shared_ptr <IntraActorCommunicatorFactory> intraActorCommFact);

public:
    static void createInstance(std::shared_ptr <ActorCommunicator> comm, std::shared_ptr <IntraActorCommunicatorFactory>);
    static SyntaxHelper * getInstance();

    std::shared_ptr <ActorCommunicator> getComm();
    std::shared_ptr <IntraActorCommunicatorFactory> getIntraActorCommFact();

    void setIntraActorComm(const std::shared_ptr<IntraActorCommunicator> & intraActorComm);
    std::shared_ptr<IntraActorCommunicator> getIntraActorComm();

    void setActorBSPlib(BSPlib * bsplib);
    BSPlib * getActorBSPlib();
};

} // namespace activebsp

#endif // __SYNTAX_HELPER_H__
