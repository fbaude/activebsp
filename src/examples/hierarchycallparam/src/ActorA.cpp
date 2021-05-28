#include "ActorA.h"

#include <iostream>
#include <vector>

#include <math.h>

#include "activebsp.h"

using namespace std;

REGISTER_ACTOR(ActorA)

ActorA::ActorA()
    : activebsp::ActorBase() {}

int ActorA::f(activebsp::Proxy<ActorB> actorB)
{
    cout << "ActorA::f()" << endl;

    return actorB.f().get();
}


