#ifndef __TEST_INSTANCIATETWICE_2_CPP__
#define __TEST_INSTANCIATETWICE_2_CPP__

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

    int foo(int v)
    {
        return v;
    }
};

DECL_ACTOR(ActorA,
          (int, foo, int)
)

REGISTER_ACTOR(ActorA)

TEST(TestMultipleInstanciate, instanciateTwice)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1});
    Proxy<ActorA> b = createActiveObject<ActorA>({2});

    int va = 42;
    int vb = 24;

    Future<int> fa = a.foo(va);
    Future<int> fb = b.foo(vb);

    int resa = fa.get();
    int resb = fb.get();

    EXPECT_EQ(va, resa);
    EXPECT_EQ(vb, resb);

    a.destroyObject();
    b.destroyObject();
}

TEST(TestMultipleInstanciate, useSameProcessTwiceSameActor)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1});

    int va = 42;

    Future<int> fa = a.foo(va);

    int resa = fa.get();

    EXPECT_EQ(va, resa);

    a.destroyObject();

    Proxy<ActorA> b = createActiveObject<ActorA>({1});

    int vb = 24;

    Future<int> fb = b.foo(vb);

    int resb = fb.get();

    EXPECT_EQ(vb, resb);

    b.destroyObject();
}

class ActorB : public ActorBase
{
public:
    ActorB() : ActorBase() {}

    int foo()
    {
        return 42;
    }
};

DECL_ACTOR(ActorB,
          (int, foo)
)

REGISTER_ACTOR(ActorB)

TEST(TestMultipleInstanciate, useSameProcessTwiceDifferentActor)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1});

    int va = 42;

    Future<int> fa = a.foo(va);

    int resa = fa.get();

    EXPECT_EQ(va, resa);

    a.destroyObject();

    Proxy<ActorB> b = createActiveObject<ActorB>({1});

    int vb = 42;

    Future<int> fb = b.foo();

    int resb = fb.get();

    EXPECT_EQ(vb, resb);

    b.destroyObject();
}


#endif // __TEST_INSTANCIATETWICE_2_CPP__
