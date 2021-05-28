#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "actormsg/CreateActorMessage.h"

using namespace std;
using namespace activebsp;


TEST(testCreateActorMessage, testCreateMessage)
{
    vector <int> pids = {2, 3, 4, 5};
    string name = "ActorA";

    CreateActorMessage msg(name, pids);

    EXPECT_EQ(INSTRUCTION_CREATE_ACTOR, msg.getInstruction()) << "instruction does not match";
    EXPECT_EQ(name, msg.getActorName()) << "CreateMessage actor name does not match";
    EXPECT_EQ(pids, msg.getPids())<< "CreateMessage pids doe not match";
}
