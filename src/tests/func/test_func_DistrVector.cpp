#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "activebsp.h"
#include "Actor.h"
#include "vector_distribution.h"
#include "DistrFuture.h"
#include "distributions/block_distribution.h"

using namespace std;
using namespace activebsp;

class ActorA : public ActorBase
{
private:
    size_t _part_size;
    std::vector<char> _part;

    void spmd_init_parts()
    {
        bsp_sync();
        _part.resize(_part_size);
        bsp_push_reg(&_part[0], _part_size);
        bsp_sync();
    }

    void spmd_distribute()
    {
        bsp_sync();
        register_single_part_result<char>(&_part[0], _part_size, bsp_pid() * _part_size);
        bsp_sync();
    }

public:
    ActorA() : ActorBase()
    {
        register_spmd(&ActorA::spmd_init_parts);
        register_spmd(&ActorA::spmd_distribute);

        bsp_push_reg(&_part_size, sizeof(size_t));
        bsp_sync();
    }

    vector_distribution<char> distribute(std::vector<char> v)
    {
        _part_size = v.size() / bsp_nprocs();

        for (int s = 1; s < bsp_nprocs(); ++s)
        {
            bsp_put(s, &_part_size, &_part_size, 0, sizeof(size_t));
        }

        bsp_run(&ActorA::spmd_init_parts);

        for (int s = 0; s < bsp_nprocs(); ++s)
        {
            bsp_put(s, &v[s * _part_size], &_part[0], 0, _part_size);
        }

        bsp_run(&ActorA::spmd_distribute);

        vector_distribution<char> dv = gather_distr_parts<char>();

        return dv;
    }

};

DECL_ACTOR(ActorA,
          (vector_distribution<char>, distribute, std::vector<char>)
)

REGISTER_ACTOR(ActorA)

class ActorB : public ActorBase
{
private:
    std::vector<char> _part;
    size_t _part_size;

    vector_distribution<char> _dv;

    void spmd_gather()
    {
        broadcast_dv(_dv);

        _part_size = _dv.getVecSize() / bsp_nprocs();
        _part.resize(_part_size);

        bsp_push_reg(&_part[0], _part_size);

        bsp_sync();

        get_part<char>(_dv, _part_size * bsp_pid(), &_part[0], _part_size);

        bsp_sync();
    }

public:
    ActorB() : ActorBase()
    {
        register_spmd(&ActorB::spmd_gather);
        bsp_sync();
    }

    std::vector<char> assemble(vector_distribution<char> dv)
    {
        _dv = dv;

        bsp_run(&ActorB::spmd_gather);

        std::vector<char> v(dv.getVecSize());

        for (int s = 0; s < bsp_nprocs(); ++s)
        {
            bsp_get(s, &_part[0], 0, &v[s * _part_size], _part_size);
        }

        bsp_seq_sync();

        return v;
    }

};

DECL_ACTOR(ActorB,
          (std::vector<char>, assemble, vector_distribution<char>)
)

REGISTER_ACTOR(ActorB)

TEST(TestDistrVectorFunc, testBasic)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1,2,3,4});

    int np = 4;
    int part_size = 10;

    std::vector<char> v(np * part_size);

    DistrFuture<char> f = a.distribute(v);

    vector_distribution<char> dv = f.get();

    ASSERT_EQ(np, dv.nparts());

    for (int i = 0; i < dv.nparts(); ++i)
    {
        vector_distribution_part_t * part = dv.getPart(i);

        ASSERT_EQ(i + 1, part->pid);
        ASSERT_EQ(part_size, part->size);
        ASSERT_EQ(i * part_size, part->offset);
    }

    a.destroyObject();
}

/*
TEST(TestDistrVectorFunc, testAssemble)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1,2,3,4});
    Proxy<ActorB> b = createActiveObject<ActorB>({5,6,7,8});

    int np = 4;
    int part_size = 10;

    std::vector<char> v(np * part_size);
    for (int i = 0; i < v.size(); ++i)
    {
        v[i] = i;
    }

    Future<vector_distribution> f1 = a.distribute(v);

    vector_distribution dv = f1.get();

    Future<std::vector<char> > f2 = b.assemble(dv);

    std::vector<char> v_out = f2.get();

    for (int i = 0; i < v.size(); ++i)
    {
        EXPECT_EQ(i, v_out[i]);
    }

    dv_release(dv);
    f1.release();
    f2.release();
    a.destroyObject();
    b.destroyObject();
}
*/

TEST(TestDistrVectorFunc, testPreDistribute)
{
    std::vector<int> pids = {1,2,3,4};
    Proxy<ActorA> a = createActiveObject<ActorA>(pids);

    int v_size = 40;

    std::vector<char> v(v_size);
    for (int i = 0; size_t(i) < v.size(); ++i)
    {
        v[i] = i;
    }

    vector_distribution<char> dv = a.block_distribute<char>(&v[0], v.size());

    int nparts = dv.nparts();
    int vecSize = dv.getVecSize();

    EXPECT_EQ(v.size(), vecSize);
    EXPECT_EQ(pids.size(), nparts);

    for (int i = 0; i < nparts; ++i)
    {
        const vector_distribution_part_t * part = dv.getPart(i);

        EXPECT_EQ(pids[i], part->pid);
        EXPECT_EQ((v_size / pids.size()) * i, part->offset);
        EXPECT_EQ(v_size / pids.size(), part->size);
    }

    std::vector<char> v_out(vecSize);

    dv_get_part<char>(dv, 0, &v_out[0], v_out.size());

    for (int i = 0; size_t(i) < v_out.size(); ++i)
    {
        EXPECT_EQ(v[i], v_out[i]);
    }

    a.destroyObject();
}



TEST(TestDistrVectorFunc, testGetPart)
{
    Proxy<ActorA> a = createActiveObject<ActorA>({1,2,3,4});

    int np = 4;
    int part_size = 10;

    std::vector<char> v(np * part_size);

    Future<vector_distribution<char> > f1 = a.distribute(v);

    vector_distribution<char> dv = f1.get();

    a.destroyObject();
}

class ActorLocalDV : public activebsp::ActorBase
{
private:

    vector_distribution<char> _dv;

    std::vector<char> _part;
    size_t _part_offset;
    size_t _part_size;

    std::vector<char> _v;
    size_t _v_size;

    void bsp_init_size()
    {

    }

    void bsp_store_vec()
    {
        bsp_sync();

        size_t end;
        div_range(_v_size, bsp_pid(), bsp_nprocs(), &_part_offset, &end);
        _part_size = end - _part_offset;
        _part.resize(_part_size);

        if (bsp_pid() != 0)
        {
            _v.resize(1);
        }

        bsp_push_reg(_v.data(), _v.size());
        bsp_sync();

        bsp_get(0, _v.data(), _part_offset, _part.data(), _part_size);
        bsp_sync();

        register_single_part_result(_part.data(), _part_size, _part_offset);
        bsp_sync();
    }

    void bsp_store_dv()
    {
        broadcast_dv(_dv);

        std::vector<char> part;

        size_t size = _dv.getVecSize();
        size_t part_offset, part_end, part_size;
        div_range(size, bsp_pid(), bsp_nprocs(), &part_offset, &part_end);
        part_size = part_end - part_offset;

        part.resize(part_size);

        dv_get_part(_dv, part_offset, part.data(), part_size);
        bsp_sync();

        register_single_part_result(part.data(), part_size, part_offset);
        bsp_sync();
    }

public:
    ActorLocalDV()
    {
        register_spmd(&ActorLocalDV::bsp_store_dv);
        register_spmd(&ActorLocalDV::bsp_init_size);
        register_spmd(&ActorLocalDV::bsp_store_vec);

        bsp_push_reg(&_v_size,      sizeof(size_t));

        bsp_push_reg(&_part_offset, sizeof(size_t));
        bsp_push_reg(&_part_size,   sizeof(size_t));

        bsp_sync();
    }

    vector_distribution<char> store_vec(std::vector<char> v)
    {
        _v = v;
        _v_size = v.size();

        for (int i = 0; i < bsp_nprocs(); ++i)
        {
            bsp_put(i, &_v_size, &_v_size, 0, sizeof(size_t));
        }

        bsp_run(&ActorLocalDV::bsp_store_vec);

        return gather_distr_parts<char>();
    }

    vector_distribution<char> store_dv(vector_distribution<char> dv)
    {
        _dv = dv;

        bsp_run(&ActorLocalDV::bsp_store_dv);

        return gather_distr_parts<char>();
    }
};

DECL_ACTOR(ActorLocalDV,
          (vector_distribution<char>, store_dv, vector_distribution<char>),
          (vector_distribution<char>, store_vec, std::vector<char>)
)

REGISTER_ACTOR(ActorLocalDV)


TEST(TestDistrVectorFunc, testGetLocal)
{
    Proxy<ActorLocalDV> a = createActiveObject<ActorLocalDV>({1,2,3,4});

    int np = 4;
    int part_size = 10;

    std::vector<char> v(np * part_size);
    for (int i = 0; size_t(i) < v.size(); ++i)
    {
        v[i] = i;
    }

    Future<vector_distribution<char> > f1 = a.store_vec(v);

    vector_distribution<char> dv = f1.get();

    //Future<vector_distribution> f2 = a.store_dv(dv);
    //f2.get();

    a.destroyObject();
}
