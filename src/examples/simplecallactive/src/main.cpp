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
    cout << "Finished creating active object ActorA" << endl;

    int v = 20;
    int d = 22;

    cout << "Calling ActorA.f()" << endl;
    Future<int> future_res1 = actorA.add(v,d);

    cout << "Asking result" << endl;
    int res = future_res1.get();

    cout << "Got result " << res << endl;

    cout << "Asking to destroy actorA" << endl;
    actorA.destroyObject();

    cout << "finalize activebsp" << endl;
    activebsp_finalize();

    cout << "finished to finalize activebsp" << endl;

    return 0;
}

