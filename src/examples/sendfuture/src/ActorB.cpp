#include "ActorB.h"

using namespace std;

REGISTER_ACTOR(ActorB)

ActorB::ActorB() : activebsp::ActorBase()
{
}

int ActorB::add_fut(activebsp::Future<int> v, int d)
{
    std::cout << "Inside ActorA::add()" << std::endl;

    std::cout << "Getting future value" << std::endl;
    int val = v.get();
    std::cout << "Got future value : " << val << std::endl;

    return val + d;
}
