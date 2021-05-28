#ifndef __ACTOR_A_H__
#define __ACTOR_A_H__

#include "Actor.h"
#include "activebsp.h"

#include "ActorB.h"

#include <vector>

class ActorA : public activebsp::ActorBase
{
public:
    ActorA();

    int f(activebsp::Proxy<ActorB> actorB);

};

DECL_ACTOR(ActorA,
          (int, f, Proxy<ActorB>)
)

#endif // __ACTOR_A_H__
