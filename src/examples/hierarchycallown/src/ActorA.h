#ifndef __ACTOR_A_H__
#define __ACTOR_A_H__

#include "Actor.h"
#include "activebsp.h"

#include "ActorB.h"

#include <vector>

class ActorA : public activebsp::ActorBase
{
private :
    activebsp::Proxy<ActorB> _actorB;

public:
    ActorA();
    ~ActorA();

    int f();

};

DECL_ACTOR(ActorA,
          (int, f)
)

#endif // __ACTOR_A_H__
