#ifndef __FUTURE_HPP__
#define __FUTURE_HPP__

#include "Future.h"

#include <sstream>
#include <string>
#include <iostream>

#include <stdlib.h>

#include <mpi.h>

#include "future/FutureKeyManager.h"
#include "serialization/serialization.h"
#include "SyntaxHelper.h"

#include "log.h"

namespace activebsp
{

template <typename Archive>
void FutureBase::save(Archive &ar, const unsigned int version) const
{
    UNUSED(version);

    ar & _dataRetriever.get();
}

template <typename Archive>
void FutureBase::load(Archive &ar, const unsigned int version)
{
    UNUSED(version);

    FutureDataRetriever * dataRetriever;
    ar & dataRetriever;
    _dataRetriever = std::shared_ptr<FutureDataRetriever>(dataRetriever);
}

template <class T>
Future<T>::Future(const std::shared_ptr<FutureDataRetriever> & dataRetriever)
    : FutureBase(dataRetriever)
{}

template <class T>
Future<T>::Future() {}

template <class T>
Future<T>::~Future() {}

template <class T>
Future<T>::Future(const Future<T> & other)
    : FutureBase(other) {}

template <class T>
Future<T> & Future<T>::operator = (const Future<T> & other)
{
    if (this != &other)
    {
        Future<T> tmp(other);
        swap(tmp);
    }

    return *this;
}

template <class T>
void Future<T>::swap(Future<T> & other)
{
    FutureBase::swap(other);
}


template <class T>
T Future<T>::get()
{
    if (!_data)
    {
        LOG_TRACE("%s", "executing get() on future");

        _data = _dataRetriever->retrieveData();
    }

    T val;
    deserialize_all<T>(&(*_data)[0], _data->size(), val);

    return val;
}


template <class T>
template <typename Archive>
void Future<T>::save(Archive &ar, const unsigned int version) const
{
    UNUSED(version);
    ar & boost::serialization::base_object<FutureBase>(*this);
}

template <class T>
template <typename Archive>
void Future<T>::load(Archive &ar, const unsigned int version)
{
    UNUSED(version);
    ar & boost::serialization::base_object<FutureBase>(*this);
}



} // namespace activebsp

#endif // __FUTURE_HPP__

