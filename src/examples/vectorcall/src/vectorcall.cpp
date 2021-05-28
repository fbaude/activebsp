#include <vector>

#include <stdio.h>
#include <time.h>

#include "activebsp.h"
#include "Actor.h"

#include "measure.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
{
public:
    ActorA() : ActorBase() {}

    std::vector<char> foo(vector<char> v)
    {
        return v;
    }
};

DECL_ACTOR(ActorA,
          (std::vector<char>, foo, std::vector<char>)
)

REGISTER_ACTOR(ActorA)

typedef enum
{
    PARAM_BEG_SIZE = 1,
    PARAM_END_SIZE,
    PARAM_DELTA_SIZE,
    PARAM_N_REPEAT,
    PARAM_OUTPUT_PATH
} args_t;

int main(int argc, char ** argv)
{
    activebsp_init(&argc, &argv);

    if (argc < PARAM_OUTPUT_PATH)
    {
        printf("Usage : %s beg_size end_size delta_size n_repeat [output_file]\n", argv[0]);
        return 1;
    }

    Proxy<ActorA> a = createActiveObject<ActorA>({1});

    int beg_size = atoi(argv[PARAM_BEG_SIZE]);
    int end_size = atoi(argv[PARAM_END_SIZE]);
    int d_size  = atoi(argv[PARAM_DELTA_SIZE]);
    int n_repeat = atoi(argv[PARAM_N_REPEAT]);

    FILE * out = stdout;
    if (argc > PARAM_OUTPUT_PATH)
    {
        out = fopen(argv[PARAM_OUTPUT_PATH], "w");
    }

    fprintf(out, "vec_size\ttime_call_s\ttime_get1_s\ttime_get2_s\n");

    for (int vec_size = beg_size; vec_size <= end_size; vec_size += d_size)
    {
        for (int i_repeat = 0; i_repeat < n_repeat; ++i_repeat)
        {
            vector<char> v(vec_size);

            struct timespec t0, t1, t2, t3, t4;
            double time_call, time_get1, time_get2;

            t0 = now();

            Future<vector<char> > f = a.foo(v);

            t1 = now();

            vector<char> res1 = f.get();

            t2 = now();

            f.resetData();

            t3 = now();

            vector<char> res2 = f.get();

            t4 = now();

            f.release();

            time_call = diff_to_sec(&t0,&t1);
            time_get1  = diff_to_sec(&t1,&t2);
            time_get2  = diff_to_sec(&t3,&t4);

            fprintf(out, "%lu\t%.10e\t%.10e\t%.10e\n", size_t(vec_size * sizeof(char)), time_call, time_get1, time_get2);
        }

    }

    if (out != stdout)
    {
        fclose(out);
    }

    a.destroyObject();

    activebsp_finalize();

    return 0;
}
