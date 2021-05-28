#ifndef __ACTOR_B_H__
#define __ACTOR_B_H__

#include "Actor.h"
#include "activebsp.h"

#include <vector>

class ActorB : public activebsp::ActorBase
{
private:
    void spmd();

    typedef enum
    {
        VEC_TAG, DELTA_TAG
    } tags;

public:
    ActorB();

    std::vector<int> multiply_all(std::vector<int> v, int d);
};

DECL_ACTOR(ActorB,
          (std::vector<int>, multiply_all, std::vector<int>, int)
)

#endif // __ACTOR_B_H__
