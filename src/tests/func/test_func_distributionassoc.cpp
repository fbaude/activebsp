#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include "activebsp.h"
#include "Actor.h"
#include "vector_distribution.h"
#include "Distributor.h"

using namespace std;
using namespace activebsp;


typedef enum
{
    BLOCK_DISTRIBUTION

} ActorA_Distributions;

class ActorA : public ActorBase
{
private:
    vector_distribution<char> _dv;

    void store_result_distribution(const distribution_t & distribution, char * buf, size_t buf_size)
    {
        int resid = store_result(&buf[0], buf_size);
        prepare_distr_result(distribution.size());

        for (int i = 0; size_t(i) < distribution.size(); ++i)
        {
            register_result_part(i, resid, distribution[i].size, distribution[i].offset);
        }
    }

    void dv_get_distribution(const vector_distribution<char> & dv, const distribution_t & distribution, char * buf)
    {
        size_t written = 0;
        for (auto it = distribution.begin(); it != distribution.end(); ++it)
        {
            dv_get_part(dv, it->offset, buf + written, it->size);
            written += it->size;
        }
    }

    void spmd()
    {
        broadcast_dv(_dv);

        size_t v_size = _dv.getVecSize();
        std::vector <char> part(v_size);

        BlockDistributor & distributor = _distributors.find(BLOCK_DISTRIBUTION)->second;

        auto distribution = distributor.distribution_part(v_size);

        dv_get_distribution(_dv, distribution, &part[0]);

        store_result_distribution(distribution, &part[0], part.size());
    }

    std::map<ActorA_Distributions, BlockDistributor> _distributors;

public:
    ActorA()
    {
        _distributors.insert(std::make_pair (BLOCK_DISTRIBUTION, BlockDistributor(bsp_pid(), bsp_nprocs())));

        register_spmd(&ActorA::spmd);
        //bsp_sync();
    }

    vector_distribution<char> foo(vector_distribution<char> dv)
    {
        _dv = dv;

        bsp_run(&ActorA::spmd);

        return gather_distr_parts<char>();
    }

};

DECL_ACTOR(ActorA,
          (vector_distribution<char>, foo, vector_distribution<char>)
)

REGISTER_ACTOR(ActorA)

template <class T, char const *str>
class DistributionRegister
{

};

char ActorA_Distributor_MyDistributor_key[] = "MyDistributor";

template <>
class DistributionRegister<ActorA, ActorA_Distributor_MyDistributor_key>
{

};


TEST(TestDistributionAssoc, testAutoDistribute)
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

    size_t vecSize = dv.getVecSize();

    std::vector<char> v_out(vecSize);

    dv_get_part(dv, 0, &v_out[0], v_out.size());

    for (int i = 0; size_t(i) < v_out.size(); ++i)
    {
        EXPECT_EQ(v[i], v_out[i]);
    }

    a.destroyObject();
}

