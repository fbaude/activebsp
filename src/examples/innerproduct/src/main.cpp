#include <iostream>
#include <vector>

#include "activebsp.h"
#include "Future.h"
#include "Proxy.h"

#include "InnerProductActor.h"

using namespace std;
using namespace activebsp;

int main()
{
    activebsp_init();

    cout << "Creating active object InnerProduct" << endl;
    Proxy <InnerProductActor> actorA = createActiveObject<InnerProductActor>(vector<int>({1,2}));

    vector<double> x(10);
    for (int i = 0; size_t(i) < x.size(); ++i)
    {
        x[i]= i+1;
    }

    cout << "Request for InnerProduct" << endl;

    Future <double> future_res = actorA.f(x,x);

    cout << "Asking result" << endl;
    double res = future_res.get();

    cout << "Result is " << res << endl;

    actorA.destroyObject();

    activebsp_finalize();
}

