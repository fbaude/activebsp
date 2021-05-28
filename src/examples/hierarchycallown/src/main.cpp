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
    Proxy <ActorA> actorA = createActiveObject<ActorA>(vector<int>({1}));

    Future <int> future_res = actorA.f();

    int res = future_res.get();

    cout << "Res : " << res << endl;

    actorA.destroyObject();

    activebsp_finalize();
}

