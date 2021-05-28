#include "ActorA.h"

#include <iostream>
#include <vector>

#include <math.h>

#include "activebsp.h"

using namespace std;

REGISTER_ACTOR(ActorA)

ActorA::ActorA()
    : activebsp::ActorBase(),
    _actorB(activebsp::createActiveObject<ActorB>(vector<int>({2})))
{

}

ActorA::~ActorA()
{
    _actorB.destroyObject();
}


int ActorA::f()
{
    cout << "ActorA::f()" << endl;

    return _actorB.f().get();
}


