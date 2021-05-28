#include "ActorA.h"

#include <iostream>
#include <vector>

#include <math.h>

using namespace std;

REGISTER_ACTOR(ActorA)

ActorA::ActorA() : activebsp::ActorBase()
{
    register_spmd(&ActorA::spmd);

    size_t tagsize = sizeof(int);
    bsp_set_tagsize(&tagsize);
    bsp_sync();
}

void ActorA::spmd()
{
    bsp_sync();

    int blocksize, d;

    int tag;
    int msg_size;
    int v_size;
    int * v;

    bsp_get_tag(&msg_size, &tag);

    for (bsp_get_tag(&msg_size, &tag); msg_size != -1; bsp_get_tag(&msg_size, &tag))
    {
        switch (tag)
        {
        case VEC_TAG:
            v_size = msg_size;
            blocksize = v_size / sizeof(int);
            v = (int *) malloc(v_size);
            bsp_move(v, v_size);
            break;

        case DELTA_TAG:
            bsp_move(&d,  sizeof(int));
            break;

        default:
            break;
        }
    }

    for (int i = 0; i < blocksize; ++i)
    {
        v[i] += d;
    }

    tag = bsp_pid();
    bsp_send(0, &tag, v, blocksize * sizeof(int));

    bsp_sync();

    free(v);
}

std::vector<int> ActorA::add_all(std::vector <int> v, int d)
{
    int blocksize = v.size() / bsp_nprocs();

    for (int i = 0, tag; i < bsp_nprocs(); ++i)
    {
        tag = VEC_TAG;
        bsp_send(i, &tag, &v[i * blocksize], blocksize * sizeof(int));

        tag = DELTA_TAG;
        bsp_send(i, &tag, &d, sizeof(int));
    }

    bsp_run(&ActorA::spmd);

    std::cout << "Returning from spmd()" << std::endl;

    std::vector <int> res(v.size());

    for (int i = 0, src, size; i < bsp_nprocs(); ++i)
    {
        bsp_get_tag(&size, &src);
        bsp_move(&res[src * blocksize], blocksize * sizeof(int));
    }

    std::cout << "Returning from f()" << std::endl;

    return res;
}


