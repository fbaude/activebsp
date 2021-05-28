#include "Future.h"


namespace activebsp
{

FutureBase::FutureBase(const std::shared_ptr<FutureDataRetriever> & dataRetriever)
    : _dataRetriever(dataRetriever)
{}

FutureBase::FutureBase() {}

FutureBase::~FutureBase() {}

FutureBase::FutureBase(const FutureBase & other)
    : _data(other._data), _dataRetriever(other._dataRetriever)
{}

FutureBase & FutureBase::operator = (const FutureBase & other)
{
    if (this != &other)
    {
        FutureBase tmp(other);
        swap(tmp);
    }

    return *this;
}

void FutureBase::swap(FutureBase & other)
{
    std::swap(_dataRetriever, other._dataRetriever);
    _data.swap(other._data);
}

void FutureBase::resetData()
{
    _data = nullptr;
}

void FutureBase::setDataRetriever(const std::shared_ptr<FutureDataRetriever> & dataRetriever)
{
    _dataRetriever = dataRetriever;
}

std::shared_ptr<FutureDataRetriever> FutureBase::getDataRetriever() const
{
    return _dataRetriever;
}


void FutureBase::release()
{
    _dataRetriever->releaseData();
}


} // namespace activebsp
