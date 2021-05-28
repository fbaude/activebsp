#include <iostream>
#include <vector>

#include "activebsp.h"
#include "Future.h"
#include "Proxy.h"

#include "ActorA.h"

using namespace std;
using namespace activebsp;

int main()
{
    activebsp_init();

    cout << "Creating active object ActorA" << endl;
    Proxy <ActorA> actorA = createActiveObject<ActorA>({1});
    Proxy <ActorB> actorB = createActiveObject<ActorB>({2});

    Future <int> future_res = actorA.f(actorB);

    int res = future_res.get();

    cout << "Res : " << res << endl;

    actorA.destroyObject();
    actorB.destroyObject();

    activebsp_finalize();
}

