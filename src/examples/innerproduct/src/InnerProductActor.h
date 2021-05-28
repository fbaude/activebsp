#ifndef __INNER_PRODUCT_ACTOR_H__
#define __INNER_PRODUCT_ACTOR_H__

#include "Actor.h"
#include "activebsp.h"

#include <vector>

class InnerProductActor : public activebsp::ActorBase
{
private:
    void spmd();
    void broadcast_data();
    void init_size();

    typedef enum
    {
        VEC_TAG, DELTA_TAG
    } tags;

    double bspip(int p, int s, int n, double *x, double *y);
    void bspinprod();

    // compute variable
    double alpha;
    double * x_p0;
    double * y_p0;
    double * x_part;
    double * y_part;
    int n_p0;
    int n_part;

public:
    InnerProductActor();

    double f(std::vector<double> x, std::vector<double> y);

};

DECL_ACTOR(InnerProductActor,
          (double, f, std::vector<double>, std::vector<double>)
)

#endif // __INNER_PRODUCT_ACTOR_H__
