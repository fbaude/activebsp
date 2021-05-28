#include <vector>

#include <stdio.h>
#include <time.h>

#include "activebsp.h"
#include "Actor.h"
#include "distributions/block_distribution.h"
#include "measure.h"

using namespace std;
using namespace activebsp;

class PipeActor : public ActorBase
{
private:
    size_t _size;
    std::vector<int> _v;

    vector_distribution<int> _vd;
    std::vector<int> _local_res;

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

        bsp_sync();

        size = _vd.getVecSize();

        div_range(size, s, p, &part_offset, &part_end);
        part_size = part_end - part_offset;

        _v.resize(part_size);

        get_part<int>(_vd, part_offset, _v.data(), part_size);

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
        bsp_push_reg(_local_res.data(), bsp_nprocs() * sizeof(int));
        bsp_sync();
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

    vector_distribution<int> fetch_dv(Future<vector_distribution<int> > vd)
    {
        _vd = vd.get();

        bsp_run(&PipeActor::bsp_fetch_dv);

        return gather_distr_parts<int>();
    }

};


DECL_ACTOR(PipeActor,
          (vector_distribution<int>, produce_dv, size_t),
          (vector_distribution<int>, fetch_dv,  Future<vector_distribution<int> >),
          (int,                      reduce_dv, Future<vector_distribution<int> >)
)

REGISTER_ACTOR(PipeActor)

int main(int argc, char ** argv)
{
    activebsp_init(&argc, &argv);

    Future<vector_distribution<int> > f1,f2,f3;

    Proxy<PipeActor> a = createActiveObject<PipeActor>({1,2});
    Proxy<PipeActor> b = createActiveObject<PipeActor>({3,4});
    Proxy<PipeActor> c = createActiveObject<PipeActor>({5,6});

    f1 = a.produce_dv(1000000);
    f2 = b.fetch_dv(f1);
    f3 = c.fetch_dv(f2);
    
    c.reduce_dv(f3).get();

    dv_release(f1.get());
    dv_release(f2.get());
    dv_release(f3.get());
    f1.release();
    f2.release();
    f3.release();

    a.destroyObject();
    b.destroyObject();
    c.destroyObject();

    activebsp_finalize();

    return 0;
}
