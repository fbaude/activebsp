#include <string>
#include <functional>

#include <gtest/gtest.h>


#include "serialization/contiguous.hpp"
#include "serialization/contiguous_serialization.hpp"

using namespace std;
using namespace activebsp;


TEST(testContiguousSerialization, testPredictSerializationSize)
{
    int vec_int_size = 10;


    vector<int> v(vec_int_size,1);
    int i = 3;

    int predicted_size = predict_contiguous_serialization_size(v,i);

    EXPECT_EQ(vec_int_size * sizeof(int) + sizeof(size_t) + sizeof(int), predicted_size);
}

TEST(testContiguousSerialization, testSerializeInt)
{
    int i = 42;

    size_t size = predict_contiguous_serialization_size(i);

    char buf[size];
    contiguous_serialize_all_to_buf(buf, size, i);

    int res = 0;
    contiguous_deserialize_all(buf, size, res);

    EXPECT_EQ(i, res);
}

TEST(testContiguousSerialization, testSerializeVectorOfInt)
{
    vector<int> vec({0,1,2,3,4,5,6,7,8,9});

    size_t size = predict_contiguous_serialization_size(vec);

    char buf[size];
    contiguous_serialize_all_to_buf(buf, size, vec);

    vector<int> res;
    contiguous_deserialize_all(buf, size, res);

    EXPECT_EQ(vec, res);

}

TEST(testContiguousSerialization, testSerializeVectorOfIntAndInt)
{
    vector<int> vec({0,1,2,3,4,5,6,7,8,9});
    int i = 42;

    size_t size = predict_contiguous_serialization_size(vec,i);

    char buf[size];
    contiguous_serialize_all_to_buf(buf, size, vec, i);

    vector<int> resvec;
    int resi;
    contiguous_deserialize_all(buf, size, resvec, resi);

    EXPECT_EQ(vec, resvec);
    EXPECT_EQ(i, resi);

}
