#include <string>
#include <functional>

#include <gtest/gtest.h>

#include <boost/serialization/serialization.hpp>

#include "serialization/serialization.h"
#include "serialization/contiguous.hpp"

using namespace std;
using namespace activebsp;


TEST(testSerialization, testFullSerialization)
{
    int arg1 = 1, arg2 = 3;

    int size = predictSerializationSize(arg1,arg2);
    std::vector<char> serial;
    serial.resize(size);

    serialize_all_to_buf(&serial[0], size, arg1, arg2);

    //std::vector<char> serial = serialize_all_to_vchar(arg1, arg2);

    std::function<int(int,int)> f ([](int a1, int a2) { return a1 + a2; });

    serial = deserialize_call_serialize<int, decltype(f), int, int>(&serial[0], serial.size(), f);

    int res;
    deserialize_all<int>(&serial[0], (int) serial.size(), res);

    EXPECT_EQ(arg1 + arg2, res) << "instruction does not match";
}

class myclass {
    std::string s;
};


struct mypodstruct
{
    int myint;
    double mydouble;
};

struct myclassstruct
{
    myclass myobj;
};

TEST(testSerialization, testIsContiguous)
{
    EXPECT_TRUE(is_contiguous<int>::value) << "int not interpreted as serializable contiguously";
    EXPECT_TRUE(is_contiguous<char>::value) << "char not interpreted as serializable contiguously";
    EXPECT_TRUE(is_contiguous<double>::value) << "double not interpreted as serializable contiguously";
    EXPECT_TRUE(is_contiguous<float>::value) << "float not interpreted as serializable contiguously";
    EXPECT_TRUE(is_contiguous<unsigned>::value) << "unsigned not interpreted as serializable contiguously";

    EXPECT_FALSE(is_contiguous<myclass>::value) << "myclass interpreted as serializable contiguously";
    EXPECT_TRUE(is_contiguous<mypodstruct>::value) << "mypodstruct not interpreted as serializable contiguously";
    EXPECT_FALSE(is_contiguous<myclassstruct>::value) << "myclassstruct interpreted as serializable contiguously";

    EXPECT_TRUE(is_contiguous<std::vector<int> >::value) << "vector of int not interpreted as serializable contiguously";
    EXPECT_FALSE(is_contiguous<std::vector<myclass> >::value) << "vector of myclass interpreted as serializable contiguously";
    EXPECT_FALSE(is_contiguous<std::vector<myclassstruct> >::value) << "vector of myclassstruct interpreted as serializable contiguously";
    EXPECT_TRUE(is_contiguous<std::vector<mypodstruct> >::value) << "vector of mypodstruct not interpreted as serializable contiguously";
}

TEST(testSerialization, testAreContiguous)
{
    EXPECT_TRUE( (are_contiguous<int, int>::value) ) << "int list not interpreted as serializable contiguously";
    EXPECT_TRUE( (are_contiguous<int, char, double>::value) ) << "pod list not interpreted as serializable contiguously";
    EXPECT_TRUE( (are_contiguous<int, std::vector<char>, std::vector<double>>::value) ) << "vector of pod and pod list not interpreted as serializable contiguously";
    EXPECT_FALSE( (are_contiguous<int, char, myclass>::value) ) << "pod list  with non pod nterpreted as serializable contiguously";

}

