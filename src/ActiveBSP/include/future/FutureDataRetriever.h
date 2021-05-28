#ifndef __FUTURE_DATA_RETRIEVER_H__
#define __FUTURE_DATA_RETRIEVER_H__

#include <memory>
#include <vector>

namespace activebsp
{

class FutureDataRetriever
{
public:
    virtual ~FutureDataRetriever() {}

    virtual std::shared_ptr<std::vector<char>> retrieveData() = 0;
    virtual void releaseData() = 0;

    template <class Archive>
    void serialize(Archive &, const unsigned int) {}
};

} // namespace activebsp

#endif // __FUTURE_DATA_RETRIEVER_H__
