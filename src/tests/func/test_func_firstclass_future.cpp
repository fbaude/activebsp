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

    int bar(Future<int> v)
    {
        return v.get() + 1;
    }
};

DECL_ACTOR(ActorA,
          (int, foo),
          (int, bar, Future<int>)
)

REGISTER_ACTOR(ActorA)

TEST(TestFirstClassFuture, testSimpleFirstClass)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1});
    Proxy<ActorA> b = createActiveObject<ActorA>({2});

    Future<int> f1 = a.foo();
    Future<int> f2 = b.bar(f1);

    int res = f2.get();

    EXPECT_EQ(43, res);

    a.destroyObject();
    b.destroyObject();
}

TEST(TestFirstClassFuture, testSelfFirstClass)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1});

    Future<int> f1 = a.foo();
    Future<int> f2 = a.bar(f1);

    int res = f2.get();

    EXPECT_EQ(43, res);

    a.destroyObject();
}
