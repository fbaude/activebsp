#include "ActorA.h"

#include <iostream>
#include <vector>

#include <math.h>

using namespace std;

REGISTER_ACTOR(ActorA)

ActorA::ActorA() : activebsp::ActorBase()
{
}

int ActorA::add(int v, int d)
{
    std::cout << "Inside ActorA::add()" << std::endl;

    return v + d;
}


