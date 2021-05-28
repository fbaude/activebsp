#ifndef __ACTOR_A_H__
#define __ACTOR_A_H__

#include "Actor.h"
#include "activebsp.h"

#include <vector>

class ActorA : public activebsp::ActorBase
{
private:
    void spmd();

    typedef enum
    {
        VEC_TAG, DELTA_TAG
    } tags;

public:
    ActorA();

    std::vector<int> add_all(std::vector<int> v, int d);

};

DECL_ACTOR(ActorA,
          (std::vector<int>, add_all, std::vector<int>, int)
)



#endif // __ACTOR_A_H__
