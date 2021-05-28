#ifndef __PROXY_HPP__
#define __PROXY_HPP__

#include "Proxy.h"

#include <string.h>

#include "future/FutureKeyManager.h"
#include "serialization/serialization.h"
#include "actormsg/instruction_envelope.h"
#include "actormsg/CallActorMessage.h"
#include "actormsg/PrefetchMessage.h"
#include "future/DirectFutureDataRetriever.h"
#include "future/LazyFutureDataRetriever.h"

namespace activebsp
{

template <class Ret, class ... Args>
Future <Ret> ProxyBase::active_call (const std::string & f, const Args & ... args)
{
    //int future_key = FutureKeyManager::getInstance()->takeKey();

    //CallActorMessage callActorMessage(f, serial_str, future_key);
    CallActorMessage callActorMessage(&_send_buf, &_send_buf_size, f, 0, args...);

    _comm->sendActorMessage(_dst, callActorMessage);
    int future_key = _comm->receiveResponseCode(_dst);

    //std::shared_ptr<FutureDataRetriever> dataRetreiver = std::make_shared<DirectFutureDataRetriever>(_comm, _dst, future_key);
    std::shared_ptr<FutureDataRetriever> dataRetreiver = std::make_shared<LazyFutureDataRetriever>(_comm, _dst, future_key);

    return Future<Ret>(dataRetreiver);
}

template <class T>
Future<vector_distribution<T> > ProxyBase::prefetch(Future<vector_distribution<T> > f, size_t size)
{
    std::shared_ptr<FutureDataRetriever> dataRetriever = f.getDataRetriever();
    std::shared_ptr<LazyFutureDataRetriever> lazyDataRetriever = std::dynamic_pointer_cast<LazyFutureDataRetriever>(dataRetriever);

    PrefetchMessage msg(lazyDataRetriever->getFuturePid(), lazyDataRetriever->getFutureKey(), size * sizeof(T));

    _comm->sendActorMessage(_dst, msg);

    int future_key = _comm->receiveResponseCode(_dst);
    std::shared_ptr<FutureDataRetriever> dataRetreiver = std::make_shared<LazyFutureDataRetriever>(_comm, _dst, future_key);

    return Future<vector_distribution<T> >(dataRetreiver);
}

template <class T>
vector_distribution<T> ProxyBase::block_distribute(const T * v, size_t n)
{
    return _comm->block_distribute(_dst, (char *) v, n, sizeof(T));
}    



} // namespace activebsp

#endif // __PROXY_HPP__
