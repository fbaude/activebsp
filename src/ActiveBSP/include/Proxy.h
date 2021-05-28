#ifndef __PROXY_H__
#define __PROXY_H__

#include <memory>

#include "Future.h"
#include "ActorCommunicator.h"

namespace activebsp
{

class ProxyBase
{
private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const \
    {
        UNUSED(version);
        ar  & _dst;
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        UNUSED(version);
        ar  & _dst;
        _comm = SyntaxHelper::getInstance()->getComm();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

protected:
    std::shared_ptr<ActorCommunicator> _comm;
    int _dst;
    char * _send_buf;
    size_t _send_buf_size;

    ProxyBase (const std::shared_ptr<ActorCommunicator> &comm, int dst);
    ProxyBase();
    ~ProxyBase();

    void setComm(const std::shared_ptr<ActorCommunicator> &comm);
    void setDst(int dst);

    template <class Ret, class ... Args>
    Future <Ret> active_call (const std::string & f, const Args & ... args);

public:
    void destroyObject();

    template <class T>
    vector_distribution<T> block_distribute(const T * v, size_t n);
    
    template <class T>
    Future<vector_distribution<T> > prefetch(Future<vector_distribution<T> > f, size_t size);

};

template <class T>
class Proxy
{
private:


public:
    Proxy();
};

} // namespace activebsp

#include "Proxy.hpp"

#endif // __PROXY_H__
