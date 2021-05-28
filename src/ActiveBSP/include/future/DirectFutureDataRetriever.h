#ifndef __DIRECT_FUTURE_DATA_RETRIEVER_H__
#define __DIRECT_FUTURE_DATA_RETRIEVER_H__

#include "FutureDataRetriever.h"
#include "ActorCommunicator.h"

namespace activebsp
{

class DirectFutureDataRetriever : public FutureDataRetriever
{
    std::shared_ptr<ActorCommunicator> _comm;
    int _src;
    int _key;

public:
    DirectFutureDataRetriever(const std::shared_ptr<ActorCommunicator> & comm, int src, int key);

    virtual ~DirectFutureDataRetriever();

    virtual std::shared_ptr<std::vector<char>> retrieveData();
    virtual void releaseData();

};

} // namespace activebsp

#endif // __DIRECT_FUTURE_DATA_RETRIEVER_H__
