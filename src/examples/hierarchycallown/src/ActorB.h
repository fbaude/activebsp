#ifndef __ACTOR_B_H__
#define __ACTOR_B_H__

#include "Actor.h"
#include "activebsp.h"

#include <vector>

class ActorB : public activebsp::ActorBase
{

public:
    ActorB();

    int f();
};

DECL_ACTOR(ActorB,
          (int, f)
)

#endif // __ACTOR_B_H__
