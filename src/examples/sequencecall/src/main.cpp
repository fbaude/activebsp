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

    cout << "Creating active object ActorA" << endl;
    Proxy <ActorA> actorA = createActiveObject<ActorA>(vector<int>({1,2}));

    cout << "Creating active object ActorB" << endl;
    Proxy <ActorB> actorB = createActiveObject<ActorB>(vector<int>({3,4}));

    int d1 = 1;
    int d2 = 2;

    vector<int> v(1000);
    for (size_t i = 0; i < v.size(); ++i)
    {
        v[i] = i;
    }

    cout << "Calling ActorA.add_all()" << endl;
    Future <vector <int> > future_resA = actorA.add_all(v,d1);

    std::vector<int> resA = future_resA.get();

    cout << "Calling ActorB.multiply_all()" << endl;
    Future <vector <int> > future_resB = actorB.multiply_all (resA, d2);

    std::vector<int> resB = future_resB.get();

    for (size_t i = 0; i < resB.size(); ++i)
    {
        if (size_t(resB[i]) != (i + d1) * d2)
        {
            cout << "Result wrong at i=" << i << " with val=" << resB[i] << endl;
        }
    }

    actorA.destroyObject();
    actorB.destroyObject();

    activebsp_finalize();
}

