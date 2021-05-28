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

TEST(TestReleaseFuture, testReleaseFuture)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1});

    Future<int> f = a.foo();

    int res = f.get();

    EXPECT_EQ(42, res);

    f.release();

    a.destroyObject();
}
