#ifndef __RESULT_PUBLISHER_H__
#define __RESULT_PUBLISHER_H__

#include <vector>

#include "actormsg/ActiveObjectRequest.h"

namespace activebsp
{

class ResultPublisher
{

public:
    virtual void publishResult(const ActiveObjectRequest &, const std::vector<char> &) = 0;

};

} // namespace activebsp

#endif // __RESULT_PUBLISHER_H__
