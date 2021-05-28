#include <iostream>
#include <vector>

#include "activebsp.h"
#include "Future.h"
#include "Proxy.h"

#include "ActorA.h"
#include "ActorB.h"

using namespace std;
using namespace activebsp;

int main()
{
    activebsp_init();

    Proxy <ActorA> actorA = createActiveObject<ActorA>(vector<int>({1}));
    Proxy <ActorB> actorB = createActiveObject<ActorB>(vector<int>({2}));

    int v = 0;
    int d = 1;

    cout << "Calling first object" << endl;
    Future<int> future_res1 = actorA.add(v,d);

    cout << "Calling second object with future" << endl;
    Future<int> future_res2 = actorB.add_fut(future_res1, d);
    cout << "Called second object with future" << endl;

    cout << "Asking result" << endl;
    int res = future_res2.get();

    cout << "Got result " << res << endl;

    actorA.destroyObject();
    actorB.destroyObject();

    activebsp_finalize();

    return 0;
}

