#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "activebsp.h"
#include "Actor.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
{
private:
    typedef enum
    {
        VEC_TAG, DELTA_TAG
    } tags;

public:
    ActorA()
        : ActorBase()
    {
        register_spmd(&ActorA::bar);
        size_t tagsize = sizeof(int);

        std::cout << "p" << bsp_pid() << "/" << bsp_nprocs() << " Setting tagsize to : " << tagsize << std::endl;

        bsp_set_tagsize(&tagsize);
        bsp_sync();
    }

    void bar()
    {
        int s,p;
        std::cout << "Calling sync()" << std::endl;
        bsp_sync();
        std::cout << "END Calling sync()" << std::endl;

        int blocksize, d;

        int tag;
        int msg_size;
        int v_size;
        int * v = NULL;

        s = bsp_pid();
        p = bsp_nprocs();

        for (bsp_get_tag(&msg_size, &tag); msg_size != -1; bsp_get_tag(&msg_size, &tag))
        {
            std::cout << "p" << s << "/" << p  << " Receiving message of tag " << tag << " and size " << msg_size << std::endl;

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
                std::cout << "p" << s << "/" << p  << " Received unknown message of tag " << tag << " and size " << msg_size << std::endl;
                bsp_move(NULL, 0);
                break;
            }
        }

        for (int i = 0; i < blocksize; ++i)
        {
            v[i] += d;
        }

        tag = bsp_pid();
        bsp_send(0, &tag, v, blocksize * sizeof(int));

        std::cout << "beg LAST sync()" << std::endl;

        bsp_sync();
        std::cout << "end LAST sync()" << std::endl;

        if (v != NULL)
        {
            free(v);
            v = NULL;
        }
    }

    std::vector<int> foo(std::vector <int> v, int d)
    {
        int blocksize = v.size() / bsp_nprocs();

        for (int i = 0, tag; i < bsp_nprocs(); ++i)
        {
            tag = VEC_TAG;
            bsp_send(i, &tag, &v[i * blocksize], blocksize * sizeof(int));

            tag = DELTA_TAG;
            bsp_send(i, &tag, &d, sizeof(int));
        }

        bsp_run(&ActorA::bar);

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
};

DECL_ACTOR(ActorA,
          (std::vector<int>, foo, std::vector<int>, int)
)

REGISTER_ACTOR(ActorA)

TEST(TestSimpleBSPCall, testSimpleBSPCall)
{
    Proxy <ActorA> actorA = createActiveObject<ActorA>(vector<int>({1,2}));

    int d1 = 2;

    vector<int> v(1000);
    for (int i = 0; size_t(i) < v.size(); ++i)
    {
        v[i] = i;
    }

    Future <vector <int> > future_res1 = actorA.foo(v,d1);
    vector<int> res1 = future_res1.get();

    for (int i = 0; size_t(i) < res1.size(); ++i)
    {
        //EXPECT_EQ(i + d1, res1[i]);
    }

    actorA.destroyObject();
}
