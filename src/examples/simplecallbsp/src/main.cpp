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
    Proxy <ActorA> actorA = createActiveObject<ActorA>(vector<int>({1,2}));
    cout << "Finished creating active object ActorA" << endl;

    int d1 = 2;

    vector<int> v(1000);
    for (int i = 0; size_t(i) < v.size(); ++i)
    {
        v[i] = i;
    }

    cout << "Calling ActorA.f()" << endl;
    Future <vector <int> > future_res1 = actorA.f(v,d1);

    cout << "Asking result" << endl;
    vector<int> res1 = future_res1.get();

    for (int i = 0; size_t(i) < res1.size(); ++i)
    {
        if (res1[i] != i + d1)
        {
            cout << "Result wrong at i=" << i << " with val=" << res1[i] << endl;
        }
    }

    actorA.destroyObject();

    activebsp_finalize();
}

