#include "ActorB.h"

#include <iostream>
#include <vector>

#include <math.h>

using namespace std;

REGISTER_ACTOR(ActorB)

ActorB::ActorB() : activebsp::ActorBase()
{

}

int ActorB::f()
{
    cout << "ActorB::f()" << endl;

    return 1;
}
