#include <string>
#include <functional>

#include <gtest/gtest.h>


#include "procassign/contiguous_assign.h"

using namespace std;
using namespace activebsp;


TEST(testContiguousAssign, testContiguousAllocate)
{
    std::vector<int> pids1 = allocate_contiguous_procs(1, 4),
                     pids2 = allocate_contiguous_procs(5, 2),
                     exppids1 = {1,2,3,4},
                     exppids2 = {5,6};

    EXPECT_EQ(exppids1, pids1);
    EXPECT_EQ(exppids2, pids2);
}

TEST(testContiguousAssign, testContiguousAllocator)
{
    ContiguousProcessAllocator allocator(1,8);


    std::vector<int> pids1 = allocator.allocate(4),
                     pids2 = allocator.allocate(3),
                     exppids1 = {1,2,3,4},
                     exppids2 = {5,6,7};

    EXPECT_EQ(exppids1, pids1);
    EXPECT_EQ(exppids2, pids2);
}
