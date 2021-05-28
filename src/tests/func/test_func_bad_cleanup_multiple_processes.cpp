#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "activebsp.h"
#include "Actor.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
{
public:
    ActorA() : ActorBase() {}

    int foo()
    {
        return 42;
    }
};

DECL_ACTOR(ActorA,
          (int, foo)
)

REGISTER_ACTOR(ActorA)

TEST(TestBadCleanupOneProcess, TestBadCleanupOneProcess)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1,2,3,4,5,6,7,8});

    Future<int> f = a.foo();

    int res = f.get();

    EXPECT_EQ(42, res);

    // Ommit destroying object
    //a.destroyObject();
}
