#include <vector>

#include <stdio.h>
#include <time.h>

#include "activebsp.h"
#include "Actor.h"
#include "distributions/block_distribution.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
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

        s = bsp_pid();
        p = bsp_nprocs();

        div_range(_size, s, p, &part_offset, &part_end);
        part_size = part_end - part_offset;

        _v.resize(part_size);

        get_part<int>(_vd, part_offset, _v.data(), part_size);

        int res = 0;

        for (size_t i = 0; i < _v.size(); ++i)
        {
            res += _v[i];
        }

        bsp_put(0, &res, _local_res.data(), bsp_pid(), sizeof(int));
        bsp_sync();
    }

public:
    ActorA() : ActorBase()
    {
        register_spmd(&ActorA::bsp_produce_dv);
        register_spmd(&ActorA::bsp_reduce_dv);

        _local_res.resize(bsp_nprocs());

        bsp_push_reg(&_size, sizeof(size_t));
        bsp_push_reg(_local_res.data(), bsp_nprocs() * sizeof(int));
        bsp_sync();
    }

    vector_distribution<int> produce_dv(size_t size)
    {
        _size = size;
        bsp_run(&ActorA::bsp_produce_dv);

        return gather_distr_parts<int>();
    }

    int reduce_dv (Future<vector_distribution<int> > f)
    {
        _vd = f.get();

        bsp_run(&ActorA::bsp_reduce_dv);

        int res = 0;

        for (int i = 0; i < bsp_nprocs(); ++i)
        {
            res += _local_res[i];
        }

        return res;
    }

};


DECL_ACTOR(ActorA,
          (vector_distribution<int>, produce_dv, size_t),
          (int, reduce_dv, Future<vector_distribution<int> >)
)

REGISTER_ACTOR(ActorA)


int main(int argc, char ** argv)
{
    activebsp_init(&argc, &argv);

    int vec_size = 40000;

    Proxy<ActorA> a = createActiveObject<ActorA>({1,2,3,4});

    Future<vector_distribution<int> > f = a.produce_dv(vec_size);

    int res = a.reduce_dv(f).get();

    std::cout << "Res : " << res << std::endl;

    a.destroyObject();

    activebsp_finalize();

    return 0;
}
