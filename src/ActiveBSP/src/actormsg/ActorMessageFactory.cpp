#include "actormsg/ActorMessageFactory.h"

#include <iostream>

#include "actormsg/instruction_envelope.h"
#include "actormsg/CreateActorMessage.h"
#include "actormsg/CallActorMessage.h"
#include "actormsg/StopActorMessage.h"
#include "actormsg/StopActorProcessMessage.h"
#include "actormsg/GetFutureDataMessage.h"
#include "actormsg/RegisterFutureMessage.h"
#include "actormsg/ReleaseFutureMessage.h"
#include "actormsg/GetResultPartsMessage.h"
#include "actormsg/StoreResultMessage.h"
#include "actormsg/ReleaseResultPartMessage.h"
#include "actormsg/DistributeVectorMessage.h"
#include "actormsg/PrefetchMessage.h"
#include "actormsg/FetchMessage.h"
#include "actormsg/StoreResultPartMessage.h"

namespace activebsp
{

std::shared_ptr <ActorMessage> ActorMessageFactory::createMessage(int src, char * buf, int bufsize)
{
    instruction_envelope_t * instruction_envelope = (instruction_envelope_t *) buf;
    char * cpy;
    switch (instruction_envelope->instruction)
    {
    case INSTRUCTION_CREATE_ACTOR:
        return std::make_shared<CreateActorMessage>(src, buf, bufsize);

    case INSTRUCTION_CALL_ACTOR:
        cpy = new char[bufsize];
        memcpy(cpy, buf, bufsize);
        return std::make_shared<CallActorMessage>(src, cpy, bufsize);

    case INSTRUCTION_DESTROY_ACTOR:
        return std::make_shared<StopActorMessage>(src, buf, bufsize);

    case INSTRUCTION_STOP:
        return std::make_shared<StopActorProcessMessage>(src, buf, bufsize);

    case INSTRUCTION_REGISTER_FUTURE_DATA:
        return std::make_shared<RegisterFutureMessage>(src, buf, bufsize);

    case INSTRUCTION_GET_FUTURE_DATA:
        return std::make_shared<GetFutureDataMessage>(src, buf, bufsize);

    case INSTRUCTION_RELEASE_FUTURE:
        return std::make_shared<ReleaseFutureMessage>(src, buf, bufsize);

    case INSTRUCTION_GET_RESULT_PARTS:
        return std::make_shared<GetResultPartsMessage>(src, buf, bufsize);

    case INSTRUCTION_STORE_RESULT:
        return std::make_shared<StoreResultMessage>(src, buf, bufsize);

    case INSTRUCTION_RELEASE_RESULT:
        return std::make_shared<ReleaseResultPartMessage>(src, buf, bufsize);

    case INSTRUCTION_DISTRIBUTE_VECTOR:
        return std::make_shared<DistributeVectorMessage>(src, buf, bufsize);

    case INSTRUCTION_PREFETCH:
        return std::make_shared<PrefetchMessage>(src, buf, bufsize);

    case INSTRUCTION_FETCH:
        return std::make_shared<FetchMessage>(src, buf, bufsize);

    case INSTRUCTION_STORE_RESULT_PART:
        return std::make_shared<StoreResultPartMessage>(src, buf, bufsize);

    default:
        std::cerr << "Warning, received unknown message with instruction tag " << instruction_envelope->instruction << std::endl;
        return nullptr;
        break;
    }
}

ActorMessageFactory * ActorMessageFactory::getInstance()
{
    if (_instance == NULL) {
        _instance = new ActorMessageFactory();
    }

    return _instance;
}

ActorMessageFactory * ActorMessageFactory::_instance = NULL;

} // namespace activebsp
