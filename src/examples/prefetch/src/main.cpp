#include <vector>

#include <stdio.h>
#include <time.h>

#include "activebsp.h"
#include "Actor.h"
#include "distributions/block_distribution.h"
#include "procassign/contiguous_assign.h"
#include "measure.h"

#define WRITE_NREQS_LOG 1
#define NREQ_LOG_FILE "queue.txt"
#define PROGRESS_PERCENT_INCREMENT 5.0
#define PRINT_TRACE 1

char queue_log_file[PATH_MAX];

using namespace std;
using namespace activebsp;

class PipeActor : public ActorBase
{
private:
    size_t _size;
    std::vector<int> _v;
    std::vector<int> _v_prefetch;

    vector_distribution<int> _vd;
    std::vector<int> _local_res;

    double _delay;

    int _ireq;

    void wait(double seconds)
    {
        struct timespec t0,t1;
        t0 = now();
        t1 = t0;
        while (diff_to_sec(&t0, &t1) < seconds) {
            t1 = now();
        }
    }

    void bsp_produce_dv()
    {
        int s,p;
        size_t part_size, part_offset, part_end;

        bsp_get(0, &_size, 0, &_size, sizeof(size_t));
        bsp_sync();

        s = bsp_pid();
        p = bsp_nprocs();

        div_range(_size, s, p, &part_offset, &part_end);
        part_size = part_end - part_offset;

        _v.resize(part_size);
        for (size_t i = 0; i < _v.size(); ++i)
        {
            _v[i] = i + part_offset;
        }

        register_single_part_result<int>(_v.data(), part_size, part_offset);
        bsp_sync();
    }

    void bsp_reduce_dv()
    {
        broadcast_dv(_vd);

        int s,p;
        size_t part_size, part_offset, part_end;
        size_t size;

        s = bsp_pid();
        p = bsp_nprocs();
        size = _vd.getVecSize();

        div_range(size, s, p, &part_offset, &part_end);
        part_size = part_end - part_offset;

        _v.resize(part_size);

        get_part<int>(_vd, part_offset, _v.data(), part_size);

        int res = 0;
        for (size_t i = 0; i < _v.size(); ++i)
        {
            res += _v[i];
        }

        bsp_put(0, &res, _local_res.data(), bsp_pid() * sizeof(int), sizeof(int));
        bsp_sync();
    }

    void bsp_fetch_dv()
    {
        int s,p;
        size_t part_size, part_offset, part_end;
        size_t size;

        s = bsp_pid();
        p = bsp_nprocs();

        broadcast_dv(_vd);

        if (s != 0)
        {
            bsp_get(0, &_delay, 0, &_delay, sizeof(double));
        }

        bsp_sync();

        size = _vd.getVecSize();

        div_range(size, s, p, &part_offset, &part_end);
        part_size = part_end - part_offset;

        _v.resize(part_size);

        get_part<int>(_vd, part_offset, _v.data(), part_size);

        wait(_delay);

        register_single_part_result<int>(_v.data(), part_size, part_offset);
        bsp_sync();
    }

    void bsp_fetch_dv_with_prefetch()
    {
        int s,p;
        size_t part_size, part_offset, part_end;
        size_t size;

        s = bsp_pid();
        p = bsp_nprocs();

        broadcast_dv(_vd);

        if (s != 0)
        {
            bsp_get(0, &_delay, 0, &_delay, sizeof(double));
        }

        bsp_sync();

        size = _vd.getVecSize();

        div_range(size, s, p, &part_offset, &part_end);
        part_size = part_end - part_offset;

        _v.resize(part_size);

        get_part<int>(_vd, part_offset, _v.data(), part_size);

        wait(_delay);

        register_single_part_result<int>(_v.data(), part_size, part_offset);
        bsp_sync();
    }


public:
    PipeActor() : ActorBase()
    {
        register_spmd(&PipeActor::bsp_produce_dv);
        register_spmd(&PipeActor::bsp_reduce_dv);
        register_spmd(&PipeActor::bsp_fetch_dv);

        _local_res.resize(bsp_nprocs());

        bsp_push_reg(&_size, sizeof(size_t));
        bsp_push_reg(&_delay, sizeof(double));
        bsp_push_reg(_local_res.data(), bsp_nprocs() * sizeof(int));
        bsp_sync();

        _ireq = 0;
    }

    vector_distribution<int> produce_dv(size_t size)
    {
        _size = size;
        bsp_run(&PipeActor::bsp_produce_dv);

        return gather_distr_parts<int>();
    }

    int reduce_dv (Future<vector_distribution<int> > f)
    {
        _vd = f.get();

        bsp_run(&PipeActor::bsp_reduce_dv);

        int res = 0;

        for (int i = 0; i < bsp_nprocs(); ++i)
        {
            res += _local_res[i];
        }

        return res;
    }

    vector_distribution<int> fetch_dv(Future<vector_distribution<int> > vd, double delay)
    {
#if WRITE_NREQS_LOG
        sprintf(queue_log_file, "P%d_%s", absp_pid(), NREQ_LOG_FILE);
        FILE * out = fopen(queue_log_file, "a");
        fprintf(out, "%d\t%d\n", _ireq++, getNPendingRequests());
        fclose(out);
#endif
        _vd = vd.get();
        _delay = delay;

        bsp_run(&PipeActor::bsp_fetch_dv);

        return gather_distr_parts<int>();
    }

    vector_distribution<int> fetch_dv_prefetch_next(Future<vector_distribution<int> > vd, double delay)
    {
#if WRITE_NREQS_LOG
        sprintf(queue_log_file, "P%d_%s", absp_pid(), NREQ_LOG_FILE);
        FILE * out = fopen(queue_log_file, "a");
        fprintf(out, "%d\t%d\n", _ireq++, getNPendingRequests());
        fclose(out);
#endif
        _vd = vd.get();
        _delay = delay;

        bsp_run(&PipeActor::bsp_fetch_dv);

        return gather_distr_parts<int>();
    }

};


DECL_ACTOR(PipeActor,
          (vector_distribution<int>, produce_dv, size_t),
          (vector_distribution<int>, fetch_dv,  Future<vector_distribution<int> >, double),
          (int,                      reduce_dv, Future<vector_distribution<int> >)
)

REGISTER_ACTOR(PipeActor)

int read_param_range(char * arg, int * beg, int * end, int * incr)
{
    size_t arg_len = strlen(arg);

    if (arg_len == 0)
    {
        return -1;
    }

    if (arg[0] == '[')
    {
        char * cur = arg + 1;

        *beg = strtol(cur, &cur, 10);
        ++cur;

        *end = strtol(cur, &cur, 10);
        ++cur;

        *incr = strtol(cur, &cur, 10);

        return 1;
    }
    else
    {
        *beg = *end = atoi(arg);
        *incr = 1;
        return 0;
    }
}

int read_param_range(char * arg, double * beg, double * end, double * incr)
{
    size_t arg_len = strlen(arg);

    if (arg_len == 0)
    {
        return -1;
    }

    if (arg[0] == '[')
    {
        char * cur = arg + 1;

        *beg = strtof(cur, &cur);
        ++cur;

        *end = strtof(cur, &cur);
        ++cur;

        *incr = strtof(cur, &cur);

        return 1;
    }
    else
    {
        *beg = *end = atof(arg);
        *incr = 1.0;
        return 0;
    }
}

enum arg_items
{
    ARG_P1 = 1,
    ARG_P2,
    ARG_p3,
    ARG_VEC_SIZE,
    ARG_N_PIPELINE_ITEMS,
    ARG_N_REPEAT,
    ARG_DELAY1,
    ARG_DELAY2,
    ARG_OUTPUT_MEASURE
};

int main(int argc, char ** argv)
{
    activebsp_init(&argc, &argv);

    int *resfetch, *resprefetch, vec_size_beg, vec_size_end, vec_size_d, n_repeat, n_pipe_items, p1,p2,p3;
    struct timespec t0,t1,t2,t3;
    double tfetch, tprefetch, delay1_beg, delay1_end, delay1_d, delay2_beg, delay2_end, delay2_d;
    char * out_path;
    double percent_completion, last_round_percent;
    ContiguousProcessAllocator proc_allocator(1, absp_nprocs() - 1);
    Future<vector_distribution<int> > f1,*f2,*f3,*fprefetch;

    Future<int> * fres;

    if (argc < ARG_OUTPUT_MEASURE)
    {
        printf("Usage: %s p1 p2 p3 vec_size pipeline_items repeat delay1 delay2 [output]\n", argv[0]);

        return 1;
    }

    p1 = atoi(argv[ARG_P1]);
    p2 = atoi(argv[ARG_P2]);
    p3 = atoi(argv[ARG_p3]);

    read_param_range(argv[ARG_VEC_SIZE], &vec_size_beg, &vec_size_end, &vec_size_d);
    n_repeat = atoi(argv[ARG_N_REPEAT]);
    n_pipe_items = atoi(argv[ARG_N_PIPELINE_ITEMS]);
    read_param_range(argv[ARG_DELAY1], &delay1_beg, &delay1_end, &delay1_d);
    read_param_range(argv[ARG_DELAY2], &delay2_beg, &delay2_end, &delay2_d);

    FILE * out_measures = stdout;

    out_path = NULL;
    if (argc > ARG_OUTPUT_MEASURE)
    {
        out_path = argv[ARG_OUTPUT_MEASURE];
        out_measures = fopen(out_path, "w");
    }

    Proxy<PipeActor> a = createActiveObject<PipeActor>(proc_allocator.allocate(p1));
    Proxy<PipeActor> b = createActiveObject<PipeActor>(proc_allocator.allocate(p2));
    Proxy<PipeActor> c = createActiveObject<PipeActor>(proc_allocator.allocate(p3));

    f2        = new Future<vector_distribution<int> > [n_pipe_items];
    f3        = new Future<vector_distribution<int> > [n_pipe_items];
    fprefetch = new Future<vector_distribution<int> > [n_pipe_items];
    fres      = new Future<int> [n_pipe_items];

    resfetch    = new int[n_pipe_items];
    resprefetch = new int[n_pipe_items];

    double expected_time_s = 0.0;
    int n_run = 0;
    for (size_t vec_size = vec_size_beg; vec_size <= size_t(vec_size_end); vec_size += vec_size_d)
    {
        for (double delay1 = delay1_beg; delay1 <= delay1_end; delay1 += delay1_d)
        {
            for (double delay2 = delay2_beg; delay2 <= delay2_end; delay2 += delay2_d)
            {
                expected_time_s += std::max(delay1, delay2) * n_repeat * n_pipe_items;
                ++n_run;
            }
        }
    }
    n_run *= n_repeat;
    last_round_percent = 0.0;

    printf("Looping over vector sizes [%d-%d-%d]\n", vec_size_beg, vec_size_end, vec_size_d);
    printf("Looping over delay1 [%f-%f-%f]\n", delay1_beg, delay1_end, delay1_d);
    printf("Looping over delay2 [%f-%f-%f]\n", delay2_beg, delay2_end, delay2_d);
    printf("Sending %d pipeline items each time\n", n_pipe_items);
    printf("Repeating each configuration %d times\n", n_repeat);
    printf("Writing measurements into file '%s'\n", out_path == NULL ? "stdout" : out_path);

    printf("According to parameters, task is expected to last at least %f seconds\n", expected_time_s);

    int i_run = 0;
    for (size_t vec_size = vec_size_beg; vec_size <= size_t(vec_size_end); vec_size += vec_size_d)
    {
        f1 = a.produce_dv(vec_size);

        int expected = 0;
        std::vector<int> full_data = dv_get_part<int>(f1.get(), 0, vec_size);

        for (size_t i = 0; i < full_data.size(); ++i)
        {
            expected += full_data[i];
        }

        for (double delay1 = delay1_beg; delay1 <= delay1_end; delay1 += delay1_d)
        {
        for (double delay2 = delay2_beg; delay2 <= delay2_end; delay2 += delay2_d)
        {

        for (int i = 0; i < n_repeat; ++i)
        {
            t0 = now();

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                f2[ipipe] = b.fetch_dv(f1, delay1);
                f3[ipipe] = c.fetch_dv(f2[ipipe], delay2);
            }

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                fres[ipipe] = c.reduce_dv(f3[ipipe]);
                resfetch[ipipe] = fres[ipipe].get();
            }

            t1 = now();

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                dv_release(f2[ipipe].get());
                dv_release(f3[ipipe].get());
                f2[ipipe].release();
                f3[ipipe].release();
                fres[ipipe].release();
            }

            t2 = now();

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                f2[ipipe] = b.fetch_dv(f1, delay1);
                fprefetch[ipipe] = c.prefetch<int>(f2[ipipe], vec_size);
                f3[ipipe] = c.fetch_dv(fprefetch[ipipe], delay2);
            }

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                fres[ipipe] = c.reduce_dv(f3[ipipe]);
                resprefetch[ipipe] = fres[ipipe].get();
            }

            t3 = now();

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                dv_release(f2[ipipe].get());
                dv_release(fprefetch[ipipe].get());
                dv_release(f3[ipipe].get());
                f2[ipipe].release();
                f3[ipipe].release();
                fprefetch[ipipe].release();
                fres[ipipe].release();
            }

            tfetch    = diff_to_sec(&t0,&t1);
            tprefetch = diff_to_sec(&t2,&t3);

            fprintf(out_measures, "%zu\t%d\t%d\t%d\t%d\t%e\t%e\t%e\t%e\n", vec_size, p1, p2, p3, n_pipe_items, delay1, delay2, tfetch, tprefetch);

            for (int ipipe = 0; ipipe < n_pipe_items; ++ipipe)
            {
                if (expected != resfetch[ipipe] || expected != resprefetch[ipipe])
                {
                    printf("Expected %d\n"
                           "fetch method : %d\n"
                           "prefetch method : %d\n", expected, resfetch[ipipe], resprefetch[ipipe]);
                }
            }

            ++i_run;
            percent_completion = (double(i_run) / double(n_run)) * 100.0;
            if (percent_completion - last_round_percent > PROGRESS_PERCENT_INCREMENT)
            {
                // ajust round percent to next threshold

                while (percent_completion - last_round_percent > PROGRESS_PERCENT_INCREMENT)
                {
                    last_round_percent += PROGRESS_PERCENT_INCREMENT;
                }

                printf("%f%%...\n", last_round_percent);

            }

        }

        }
        }

        dv_release(f1.get());
        f1.release();
    }

    a.destroyObject();
    b.destroyObject();
    c.destroyObject();

    delete[] f2;
    delete[] f3;
    delete[] fprefetch;
    delete[] fres;
    delete[] resfetch;
    delete[] resprefetch;

    activebsp_finalize();

    return 0;
}
