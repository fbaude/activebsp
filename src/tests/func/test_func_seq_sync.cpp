#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "activebsp.h"
#include "Actor.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
{
private:
    int _val;

public:
    ActorA() : ActorBase()
    {
        bsp_push_reg(&_val, sizeof(int));
        bsp_sync();
    }


    int foo(int newval)
    {
        int rcvval = 0;
        bsp_put(1, &newval, &_val, 0, sizeof(int));
        bsp_seq_sync();
        bsp_get(1, &_val, 0, &rcvval, sizeof(int));
        bsp_seq_sync();

        return rcvval;
    }
};

DECL_ACTOR(ActorA,
          (int, foo, int)
)

REGISTER_ACTOR(ActorA)

TEST(TestSeqSync, testSimpleSeqSync)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1,2});

    int newval = 1;

    Future<int> f = a.foo(newval);

    int res = f.get();

    EXPECT_EQ(newval, res);

    a.destroyObject();
}
