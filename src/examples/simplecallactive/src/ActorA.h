#ifndef __ACTOR_A_H__
#define __ACTOR_A_H__

#include "Actor.h"
#include "activebsp.h"

#include <vector>

class ActorA : public activebsp::ActorBase
{

public:
    ActorA();

    int add(int v, int d);
};

DECL_ACTOR(ActorA,
          (int, add, int, int)
)

#endif // __ACTOR_A_H__
