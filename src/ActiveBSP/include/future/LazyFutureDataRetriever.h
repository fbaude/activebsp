#ifndef __LAZY_FUTURE_DATA_RETRIEVER_H__
#define __LAZY_FUTURE_DATA_RETRIEVER_H__

#include "FutureDataRetriever.h"

#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/base_object.hpp>

#include "ActorCommunicator.h"
#include "SyntaxHelper.h"

namespace activebsp
{

class LazyFutureDataRetriever : public FutureDataRetriever
{
private:
    std::shared_ptr<ActorCommunicator> _comm;
    int _key;
    int _pid;

    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int) const
    {
        ar & boost::serialization::base_object<FutureDataRetriever>(*this);
        ar  & _key;
        ar  & _pid;
    }

    template<class Archive>
    void load(Archive & ar, const unsigned int)
    {
        ar & boost::serialization::base_object<FutureDataRetriever>(*this);
        ar  & _key;
        ar  & _pid;
        _comm = SyntaxHelper::getInstance()->getComm();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
    LazyFutureDataRetriever(const std::shared_ptr<ActorCommunicator> & comm, int pid, int key);
    LazyFutureDataRetriever();

    virtual ~LazyFutureDataRetriever();

    virtual std::shared_ptr<std::vector<char>> retrieveData();
    virtual void releaseData();

    int getFuturePid() const;
    int getFutureKey() const;
};

} // namespace activebsp

#endif // __LAZY_FUTURE_DATA_RETRIEVER_H__
