#include <vector>

#include <stdio.h>
#include <time.h>

#include "activebsp.h"
#include "Actor.h"
#include "DistrFuture.h"

#include "measure.h"
#include "distributions/block_distribution.h"


using namespace std;
using namespace activebsp;

#define MEASURE_OBJECT_INTERNAL 0

class RelayActor : public ActorBase
{
private:
    vector<char> r_v_part;

    int r_v_size;

    vector_distribution<char> _dv;

#if MEASURE_OBJECT_INTERNAL
    FILE * _out_p0 = stdout;
    FILE * _out_seq = stdout;
#endif
public:
    RelayActor() : ActorBase()
    {
        register_spmd(&RelayActor::init_size);
        register_spmd(&RelayActor::bsp_fetch);
        register_spmd(&RelayActor::call_sync);
        register_spmd(&RelayActor::end_sync);
        register_spmd(&RelayActor::bsp_fetch_dv);

        bsp_push_reg(&r_v_size, sizeof(int));
        bsp_sync();
    }

    ~RelayActor()
    {
#if MEASURE_OBJECT_INTERNAL
        if (_out_p0 != stdout)
        {
            fclose(_out_p0);
        }
        if (_out_seq != stdout)
        {
            fclose(_out_seq);
        }
#endif
    }

    void init_size()
    {
#if MEASURE_OBJECT_INTERNAL
        struct timespec t0, t1, t2, t3, t4;

        t0=now();
#endif
        bsp_sync();
#if MEASURE_OBJECT_INTERNAL
        t1=now();
#endif
        r_v_part = vector<char>(1 + r_v_size / bsp_nprocs());
#if MEASURE_OBJECT_INTERNAL
        t2=now();
#endif
        bsp_push_reg(&r_v_part[0], r_v_part.size());
#if MEASURE_OBJECT_INTERNAL
        t3=now();
#endif
        bsp_sync();
#if MEASURE_OBJECT_INTERNAL
        t4=now();

        if (bsp_pid() == 0)
        {
            fprintf(_out_p0, "%d\t%f\t%f\t%f\t%f",
                    r_v_size,
                    diff_to_sec(&t0,&t1),
                    diff_to_sec(&t1,&t2),
                    diff_to_sec(&t2,&t3),
                    diff_to_sec(&t3,&t4)
            );
        }
#endif
    }

    void call_sync()
    {
#if MEASURE_OBJECT_INTERNAL
        struct timespec t0, t1;

        t0=now();
#endif
        bsp_sync();
#if MEASURE_OBJECT_INTERNAL
        t1=now();

        if (bsp_pid() == 0)
        {
            fprintf(_out_p0, "\t%f", diff_to_sec(&t0,&t1));
        }
#endif
    }

    void end_sync()
    {
#if MEASURE_OBJECT_INTERNAL
        struct timespec t0, t1;

        t0=now();
#endif
        bsp_sync();
#if MEASURE_OBJECT_INTERNAL
        t1=now();

        if (bsp_pid() == 0)
        {
            fprintf(_out_p0, "\t%f\n", diff_to_sec(&t0,&t1));
        }
#endif
    }

    vector_distribution<char> fetch_df(DistrFuture<char> df)
    {
        return fetch_dv(df.get());
    }

    vector_distribution<char> fetch_dv(vector_distribution<char> dv)
    {
        _dv = dv;

        bsp_run(&RelayActor::bsp_fetch_dv);

        return gather_distr_parts<char>();
    }

    std::vector<char> fetch_firstclass(Future<vector<char> > v)
    {
        return fetch(v.get());
    }

    std::vector<char> fetch(vector<char> v)
    {
#if MEASURE_OBJECT_INTERNAL
        struct timespec t0, t1, t2, t3, t4, t5, t6, t7;

        t0 = now();
#endif

        int v_size = v.size();
        for (int s = 0; s < bsp_nprocs(); ++s)
        {
            bsp_put(s, &v_size, &r_v_size, 0, sizeof(int));
        }

#if MEASURE_OBJECT_INTERNAL
        t1 = now();
#endif

        bsp_run(&RelayActor::init_size);

#if MEASURE_OBJECT_INTERNAL
        t2 = now();
#endif

        for (int s = 0; s < bsp_nprocs(); ++s)
        {
            size_t beg, end,size;
            div_range(v.size(), s, bsp_nprocs(), &beg, &end);
            size = end - beg;

            bsp_put(s, &v[beg], &r_v_part[0], 0, size * sizeof(char));
        }

#if MEASURE_OBJECT_INTERNAL
        t3 = now();
#endif

        bsp_run(&RelayActor::bsp_fetch);

#if MEASURE_OBJECT_INTERNAL
        t4 = now();
#endif

        for (int s = 0; s < bsp_nprocs(); ++s)
        {
            size_t beg, end,size;
            div_range(v.size(), s, bsp_nprocs(), &beg, &end);
            size = end - beg;

            bsp_get(s, &r_v_part[0], 0, &v[beg], size);
        }

#if MEASURE_OBJECT_INTERNAL
        t5 = now();
#endif

        bsp_run(&RelayActor::call_sync);

#if MEASURE_OBJECT_INTERNAL
        t6 = now();
#endif

        bsp_run(&RelayActor::end_sync);

#if MEASURE_OBJECT_INTERNAL
        t7 = now();

        fprintf(_out_seq, "%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\n",
                r_v_size,
                bsp_nprocs(),
                diff_to_sec(&t0,&t1),
                diff_to_sec(&t1,&t2),
                diff_to_sec(&t2,&t3),
                diff_to_sec(&t3,&t4),
                diff_to_sec(&t4,&t5),
                diff_to_sec(&t5,&t6),
                diff_to_sec(&t6,&t7)
        );
#endif

        return v;
    }

    void bsp_fetch_dv()
    {
        broadcast_dv(_dv);

        r_v_size = _dv.getVecSize();
        r_v_part = vector<char>(r_v_size / bsp_nprocs());
        bsp_push_reg(&r_v_part[0], r_v_part.size());
        bsp_sync();

        size_t beg, end,size;
        div_range(r_v_size, bsp_pid(), bsp_nprocs(), &beg, &end);
        size = end - beg;

        dv_get_part(_dv, beg, &r_v_part[0], size);

        register_single_part_result<char>(&r_v_part[0], size, beg);
        bsp_sync();
    }

    void bsp_fetch()
    {
#if MEASURE_OBJECT_INTERNAL
        struct timespec t0, t1, t2;

        t0 = now();
#endif
        bsp_sync();
#if MEASURE_OBJECT_INTERNAL
        t1 = t2 = now();

        if (bsp_pid() == 0)
        {
            fprintf(_out_p0, "\t%f\t%f",
                    diff_to_sec(&t0,&t1),
                    diff_to_sec(&t1,&t2)
            );
        }
#endif
    }

    int openLogs(std::string prefix)
    {
#if MEASURE_OBJECT_INTERNAL
        _out_seq = fopen((prefix + "_seq").c_str(), "a");
        _out_p0  = fopen((prefix + "_p0") .c_str(), "a");

        size_t size;
        fseek(_out_seq, 0, std::ios_base::end);
        size = ftell(_out_seq);
        fseek(_out_seq, 0, std::ios_base::beg);

        if (size == 0)
        {
            fprintf(_out_seq, "#vec_size\tp\tbcast_init_size\tbsprun_init_size\tscatter_puts\tbsprun_add\tgather_gets\tbsprun_call_sync\tbsprun_end_sync\n");
        }

        fseek(_out_p0, 0, std::ios_base::end);
        size = ftell(_out_p0);
        fseek(_out_p0, 0, std::ios_base::beg);

        if (size == 0)
        {
            fprintf(_out_p0, "#vec_size\tinit_size_sync_sizes\tinit_size_alloc_parts\tinit_size_pushregs\tinit_size_sync_regs\tbsp_add_syncgather\tbsp_add_additions\tcall_sync_sync\tend_sync_sync\n");
        }
#else
        UNUSED(prefix);
#endif

        return 0;
    }

};

DECL_ACTOR(RelayActor,
          (std::vector<char>, fetch,               std::vector<char>),
          (std::vector<char>, fetch_firstclass, Future<std::vector<char> >),
          (vector_distribution<char>, fetch_dv, vector_distribution<char>),
          (vector_distribution<char>, fetch_df, DistrFuture<char>),
          (int, openLogs, std::string)
)

REGISTER_ACTOR(RelayActor)


std::vector<int> assignProcs(int offset, int p)
{
    std::vector<int> pids(p);
    for (int i = 0; i < p; ++i)
    {
        pids[i] = offset + i;
    }

    return pids;
}

typedef enum
{
    PARAM_PA = 1,
    PARAM_PB,
    PARAM_BEG_SIZE,
    PARAM_END_SIZE,
    PARAM_DELTA_SIZE,
    PARAM_N_REPEAT,
    PARAM_LOG_FILE

} parameter_t;

int main(int argc, char ** argv)
{
    activebsp_init(&argc, &argv);

    if (argc < 7 || argc > 8)
    {
        printf("Usage : %s pA pB beg_size end_size interval n_repeat [output_file]\n", argv[0]);
        return 1;
    }

    int pA = atoi(argv[PARAM_PA]);
    int pB = atoi(argv[PARAM_PB]);
    int beg_size = atoi(argv[PARAM_BEG_SIZE]);
    int end_size = atoi(argv[PARAM_END_SIZE]);
    int d_size  = atoi(argv[PARAM_DELTA_SIZE]);
    int n_repeat = atoi(argv[PARAM_N_REPEAT]);

    Proxy<RelayActor> a = createActiveObject<RelayActor>(assignProcs(1,pA));
    Proxy<RelayActor> b = createActiveObject<RelayActor>(assignProcs(1 + pA, pB));

    FILE * out = stdout;
    size_t size = 0;
    if (argc > 5)
    {
        char * logfile = argv[PARAM_LOG_FILE];
        out = fopen(logfile, "a");

        fseek(out, 0, std::ios_base::end);
        size = ftell(out);
        fseek(out, 0, std::ios_base::beg);

        a.openLogs(std::string(logfile) + "_ObjectA").release();
        b.openLogs(std::string(logfile) + "_ObjectB").release();
    }

    if (size == 0)
    {
        fprintf(out, "#vec_size\tpA\tpB\ttime_direct\ttime_firstclass\ttime_dv\ttime_dv_distribute\ttime_dv_firstadd\ttime_dv_secondadd\ttime_dv_gather\n");
    }

    int last_size = beg_size;
    for (int vec_size = beg_size; vec_size <= end_size; last_size = vec_size, vec_size += d_size);
    {
        // Warm up
        int vec_size = last_size;
        vector<char> v(vec_size, 1);

        Future<vector<char>> f1 = a.fetch(v);

        Future<vector<char>> f2 = b.fetch(f1.get());

        vector<char> res1 = f2.get();

        Future<vector<char>> f3 = a.fetch(v);
        Future<vector<char>> f4 = b.fetch_firstclass(f3);

        vector<char> res2 = f4.get();

        vector_distribution<char> dv1 = a.block_distribute(&v[0], v.size());

        Future<vector_distribution<char> > f5 = a.fetch_dv(dv1);
        vector_distribution<char> dv2 = f5.get();

        Future<vector_distribution<char> > f6 = b.fetch_dv(dv2);
        vector_distribution<char> dv3 = f6.get();

        std::vector<char> res3(dv3.getVecSize());
        dv_get_part(dv3, 0, &res3[0], res3.size());

        f1.release();
        f2.release();
        f3.release();
        f4.release();
        f5.release();
        f6.release();
        dv_release(dv1);
        dv_release(dv2);
        dv_release(dv3);
    }

    for (int vec_size = beg_size; vec_size <= end_size; vec_size += d_size)
    {
        for (int i_repeat = 0; i_repeat < n_repeat; ++i_repeat)
        {
            vector<char> v(vec_size, 1);

            struct timespec t0, t1, t2, t3, t6, t7, t8,t9,t10;
            double time_direct, time_firstclass, time_dv, time_dv_distribute, time_dv_firstadd, time_dv_secondadd, time_dv_gather;

            t0 = now();

            Future<vector<char>> f1 = a.fetch(v);
            
            t1 = now();
            
            Future<vector<char>> f2 = b.fetch(f1.get());
            
            t2 = now();

            vector<char> res1 = f2.get();

            t3 = now();

            Future<vector<char>> f3 = a.fetch(v);          
            Future<vector<char>> f4 = b.fetch_firstclass(f3);
            
            vector<char> res2 = f4.get();

            t6 = now();

            vector_distribution<char> dv1 = a.block_distribute(&v[0], v.size());

            t7 = now();

            DistrFuture<char> f5 = a.fetch_dv(dv1);

            t8 = now();

            DistrFuture<char> f6 = b.fetch_df(f5);
            vector_distribution<char> dv3 = f6.get();

            t9 = now();

            std::vector<char> res3(dv3.getVecSize());
            dv_get_part(dv3, 0, &res3[0], res3.size());

            t10 = now();

            dv_release(dv1);
            dv_release(f5.get());
            dv_release(dv3);
            f1.release();
            f2.release();
            f3.release();
            f4.release();
            f5.release();
            f6.release();

            time_direct     = diff_to_sec(&t0,&t3);
            time_firstclass = diff_to_sec(&t3,&t6);
            time_dv         = diff_to_sec(&t6,&t10);
            time_dv_distribute = diff_to_sec(&t6,&t7);
            time_dv_firstadd = diff_to_sec(&t7,&t8);
            time_dv_secondadd = diff_to_sec(&t8,&t9);
            time_dv_gather = diff_to_sec(&t9,&t10);

            fprintf(out, "%lu\t%d\t%d\t%.10e\t%.10e\t%.10e\t%.10e\t%.10e\t%.10e\t%.10e\n", vec_size * sizeof(char), pA, pB,
                    time_direct,
                    time_firstclass,
                    time_dv,
                    time_dv_distribute,
                    time_dv_firstadd,
                    time_dv_secondadd,
                    time_dv_gather
            );
        }

    }

    if (out != stdout)
    {
        fclose(out);
    }

    a.destroyObject();
    b.destroyObject();

    activebsp_finalize();

    return 0;
}
