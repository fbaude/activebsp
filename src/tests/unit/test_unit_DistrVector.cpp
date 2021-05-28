#include <string>
#include <vector>

#include "vector_distribution.h"
#include "serialization/serialization.h"

#include <gtest/gtest.h>


using namespace std;
using namespace activebsp;

TEST(testDistrVector, testConstructor)
{
    int nparts = 5;

    vector_distribution<char> vec(nparts);

    EXPECT_EQ(nparts, vec.nparts()) << "vector_distribution constructor nparts element does not match parameter";
}

TEST(testDistrVector, testAddSinglePart)
{
    int key = 1;
    int pid = 2;
    int resid = 3;

    size_t size = 10;
    std::vector<char> v(size);
    size_t offset = 20;


    vector_distribution<char> vec(key);

    vec.register_part(0, pid, resid, size, offset);
    vector_distribution_part_t * part = vec.getPart(0);

    EXPECT_EQ(pid, part->pid) << "vector_distribution register_part did not put right pid element as retreived in getPart()";
    EXPECT_EQ(resid, part->resid) << "vector_distribution register_part did not put right resid element as retreived in getPart()";
    EXPECT_EQ(size, part->size) << "vector_distribution register_part did not put right size element as retreived in getPart()";
    EXPECT_EQ(offset, part->offset) << "vector_distribution register_part did not put right offset element as retreived in getPart()";

}

TEST(testDistrVector, testSerialization)
{
    int key = 1;
    int pid = 2;
    int resid = 3;

    size_t size = 10;
    std::vector<char> v(size);
    size_t offset = 20;

    vector_distribution<char> dv(key);

    dv.register_part(0, pid, resid, size, offset);

    std::vector<char> serial_buf = serialize_all_to_vchar(dv);

    vector_distribution<char> dv_out;
    deserialize_all<vector_distribution<char> > (&serial_buf[0], serial_buf.size(), dv_out);

    vector_distribution_part_t * part = dv_out.getPart(0);

    EXPECT_EQ(pid, part->pid) << "vector_distribution register_part did not put right pid element as retreived in getPart()";
    EXPECT_EQ(resid, part->resid) << "vector_distribution register_part did not put right resid element as retreived in getPart()";
    EXPECT_EQ(size, part->size) << "vector_distribution register_part did not put right size element as retreived in getPart()";
    EXPECT_EQ(offset, part->offset) << "vector_distribution register_part did not put right offset element as retreived in getPart()";

}

TEST(testDistrVector, testAddMultiplePart)
{
    int pid = 2;
    int nparts = 2;

    size_t size = 10;

    std::vector<char> v1(size);
    std::vector<char> v2(size);

    vector_distribution<char> vec(nparts);

    for (int i = 0; i < nparts; ++i)
    {
        vec.register_part(i, pid + i, i, size, i * size);
    }

    EXPECT_EQ(nparts, vec.nparts()) << "vector_distribution constructor key does not match getKey()";

    for (int i = 0; i < nparts; ++i)
    {
        vector_distribution_part_t * part = vec.getPart(i);

        EXPECT_EQ(pid + i, part->pid) << "vector_distribution register_part did not put right pid element as retreived in getPart()";
        EXPECT_EQ(i, part->resid) << "vector_distribution register_part did not put right resid element as retreived in getPart()";
        EXPECT_EQ(size, part->size) << "vector_distribution register_part did not put right size element as retreived in getPart()";
        EXPECT_EQ(i * size, part->offset) << "vector_distribution register_part did not put right offset element as retreived in getPart()";
    }
}

