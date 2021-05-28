#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "actormsg/CallActorMessage.h"
#include "serialization/serialization.h"

using namespace std;
using namespace activebsp;


TEST(testCallActorMessage, testCreateMessage)
{
    string function_name = "ActorA";
    std::string args_str = "some args";
    std::vector<char> args(args_str.begin(), args_str.end());
    int future_key = 42;

    CallActorMessage msg(function_name, args, future_key);

    EXPECT_EQ(INSTRUCTION_CALL_ACTOR, msg.getInstruction()) << "instruction does not match";
    EXPECT_EQ(function_name, msg.getFunctionName()) << "Function name does not match";
    EXPECT_EQ(args, msg.getArgs()) << "Args do not match";
    EXPECT_EQ(future_key, msg.getFutureKey())<< "Future key does not match";
}

TEST(testCallActorMessage, testCreateMessageVargs)
{
    string function_name = "ActorA";
    std::string arg1 = "a string";
    int arg2 = 24;
    int future_key = 42;

    std::string out_arg1;
    int out_arg2;

    CallActorMessage msg(function_name, future_key, arg1, arg2);

    EXPECT_EQ(INSTRUCTION_CALL_ACTOR, msg.getInstruction()) << "instruction does not match";
    EXPECT_EQ(function_name, msg.getFunctionName()) << "Function name does not match";
    EXPECT_EQ(future_key, msg.getFutureKey())<< "Future key does not match";

    deserialize_all(msg.getArgsPtr(), msg.getArgsSize(), out_arg1, out_arg2);

    EXPECT_EQ(arg1, out_arg1) << "arg1 does not match";
    EXPECT_EQ(arg2, out_arg2) << "Arg2 does not match";
}

TEST(testCallActorMessage, testCreateMessageVargsReuseBuf)
{
    string function_name = "ActorA";
    std::string arg1 = "a string";
    int arg2 = 24;
    int future_key = 42;

    std::string out_arg1;
    int out_arg2;
    char * buf = NULL;
    size_t buf_size = 0;

    CallActorMessage msg(&buf, &buf_size, function_name, future_key, arg1, arg2);

    EXPECT_EQ(INSTRUCTION_CALL_ACTOR, msg.getInstruction()) << "instruction does not match";
    EXPECT_EQ(function_name, msg.getFunctionName()) << "Function name does not match";
    EXPECT_EQ(future_key, msg.getFutureKey())<< "Future key does not match";

    deserialize_all(msg.getArgsPtr(), msg.getArgsSize(), out_arg1, out_arg2);

    EXPECT_EQ(arg1, out_arg1) << "arg1 does not match";
    EXPECT_EQ(arg2, out_arg2) << "Arg2 does not match";
}
