#ifndef __FUTURE_H__
#define __FUTURE_H__

#include <memory>

#include <boost/serialization/split_member.hpp>

#include "ActorCommunicator.h"
#include "future/FutureDataRetriever.h"

namespace activebsp
{

class FutureBase
{
protected:
    std::shared_ptr <std::vector<char>> _data;

    std::shared_ptr<FutureDataRetriever> _dataRetriever;

    friend class boost::serialization::access;

    template <typename Archive>
    void save(Archive &ar, const unsigned int version) const;

    template <typename Archive>
    void load(Archive &ar, const unsigned int version);

    BOOST_SERIALIZATION_SPLIT_MEMBER()


public:
    FutureBase(const std::shared_ptr<FutureDataRetriever> & dataRetriever);
    FutureBase();

    FutureBase(const FutureBase & other);

    void swap(FutureBase & other);

    FutureBase & operator = (const FutureBase & other);

    void resetData();

    void setDataRetriever(const std::shared_ptr<FutureDataRetriever> & dataRetriever);
    std::shared_ptr<FutureDataRetriever> getDataRetriever() const;

    ~FutureBase();

    void release();
};

template <class T>
class Future : public FutureBase
{
protected:
    friend class boost::serialization::access;

    template <typename Archive>
    void save(Archive &ar, const unsigned int version) const;

    template <typename Archive>
    void load(Archive &ar, const unsigned int version);

    BOOST_SERIALIZATION_SPLIT_MEMBER()

public:
    Future(const std::shared_ptr<FutureDataRetriever> & dataRetriever);
    Future();
    Future(const Future<T> & other);

    Future<T> & operator = (const Future<T> & other);

    void swap( Future<T> & other);

    ~Future();

    T get();
};

} // namespace activebsp

#include "future/Future.hpp"

#endif // __FUTURE_H__
