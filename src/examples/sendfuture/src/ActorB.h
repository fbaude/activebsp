#ifndef __ACTOR_B_H__
#define __ACTOR_B_H__

#include "Actor.h"
#include "activebsp.h"
#include "Future.h"

class ActorB : public activebsp::ActorBase
{

public:
    ActorB();

    int add_fut(activebsp::Future<int> v, int d);
};

DECL_ACTOR(ActorB,
          (int, add_fut, Future<int>, int)
)

#endif // __ACTOR_B_H__
