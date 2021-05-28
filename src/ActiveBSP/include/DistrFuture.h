#ifndef __DISTR_FUTURE_H__
#define __DISTR_FUTURE_H__

#include "Future.h"
#include "vector_distribution.h"

namespace activebsp
{

template <typename T>
using DistrFuture = Future<vector_distribution<T> >;

} // namespace activebsp

#endif
