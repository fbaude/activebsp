#include <vector>
#include <iostream>

#include <mpi.h>

#include <gtest/gtest.h>

#include "activebsp.h"
#include "Actor.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
{
public:
    ActorA() : ActorBase() {}

    int foo(int a)
    {
        sleep(1);
        return a;
    }
};

DECL_ACTOR(ActorA,
          (int, foo, int)
)

REGISTER_ACTOR(ActorA)

TEST(TestMultipleCall, testmultiplecall)
{
    double t1,t2;

    int a1 = 1, a2 = 2;

    Proxy<ActorA> a = createActiveObject<ActorA>({1});

    Future<int> f1 = a.foo(1);

    t1 = MPI_Wtime();

    Future<int> f2 = a.foo(2);

    t2 = MPI_Wtime();

    int res1 = f1.get();
    int res2 = f2.get();

    EXPECT_EQ(a1, res1);
    EXPECT_EQ(a2, res2);

    a.destroyObject();

    EXPECT_LT(t2 - t1, 0.5);
}
