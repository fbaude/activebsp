#include <string>
#include <vector>
#include <functional>

#include <gtest/gtest.h>

#include "ActiveObjectRequest.h"
#include "actormsg/CallActorMessage.h"
#include "serialization.h"

using namespace std;
using namespace activebsp;

int add (int v, int d)
{
    return v + d;
}

TEST(testCallFlow, testCallFlow)
{
    int v = 22, d = 20;
    string f_name = "add";
    int future_key = 1;
    int src = 3;

    std::vector<char> serial_args = serialize_all_to_vchar(v,d);

    CallActorMessage sentMessage(f_name, serial_args, future_key);

    ActiveObjectRequest req (std::make_shared<CallActorMessage>(src, sentMessage.getData(), sentMessage.getDataSize()), src);

    EXPECT_EQ(req.getSource(), req.getSource());

    std::shared_ptr<CallActorMessage> receivedMessage(req.getCallActorMessage());

    std::vector<char> received_args = receivedMessage->getArgs();

    EXPECT_EQ(f_name, receivedMessage->getFunctionName());
    EXPECT_EQ(future_key, receivedMessage->getFutureKey());
    EXPECT_EQ(serial_args, received_args);

    std::function<int(int,int)> f(add);

    std::vector<char> serial_res = deserialize_call_serialize<int, decltype(f), int, int>(&received_args[0], received_args.size(), f);

    int res;
    deserialize_all<int>(&serial_res[0], serial_res.size(), res);

    EXPECT_EQ(v + d, res);
}
