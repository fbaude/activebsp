#ifndef __ACTOR_H__
#define __ACTOR_H__

#include <vector>
#include <map>
#include <string>
#include <functional>
#include <iostream>
#include <memory>

#include <stdint.h>

#include <boost/any.hpp>

#include <mpi.h>

#include "ActorCommunicator.h"
#include "ActiveBSPlib.h"
#include "IntraActorCommunicator.h"
#include "vector_distribution.h"
#include "management/MasterProxy.h"

namespace activebsp
{

class ActorBase
{
private:
    std::map <uint64_t, int > _spmd_function_ids;
    std::vector <std::function<void()> > _spmd_functions;
    int _reqId;

    std::shared_ptr<ActorCommunicator> _actorComm;
    std::shared_ptr<IntraActorCommunicator> _intraActorComm;
    std::shared_ptr<MasterProxy> _masterProxy;

    vector_distribution_base _cur_vector_distribution;
    int _cur_vector_distribution_part_size = 0;

    std::vector<char> _dv_buf;
    int _dv_buf_nelems;

    int _next_resid = 0;
    std::map<int, vector_distribution_base> _RequestDistrVectors;

    int _next_spmd_function_id;

    void run_function_id(int id);

protected:
    BSPlib * _bsplib;

public:
    ActorBase();

    void setIntraActorComm(const std::shared_ptr<IntraActorCommunicator> & intraActorComm);
    void setMasterProxy(const std::shared_ptr<MasterProxy> & MasterProxy);

    void call_spmd(int function_id);

    void kill_slaves();

    bool is_master();

    void setRequestId(int reqId);
    int getRequestId() const;

    int getNPendingRequests();

    template <class T>
    void register_spmd (void (T::*spmd_function)());

    template <class T>
    void bsp_run (void (T::*spmd_function)());

    std::vector<char> get_part(const vector_distribution_base & dv, size_t offset, size_t size);
    
    void get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size);

    template <class T>
    std::vector<T> get_part(const vector_distribution<T> & dv, size_t offset, size_t size);

    template <class T>
    void get_part(const vector_distribution<T> & dv, size_t offset, T * out_buf, size_t size);

    void broadcast_dv(vector_distribution_base & dv);

    int store_result(const char * buf, size_t size);

    template <class T>
    int store_result(const T * buf, size_t size);

    void register_single_part_result(const char *, size_t size, size_t offset);

    template <class T>
    void register_single_part_result(const T *, size_t size, size_t offset);

    template <class T>
    T * getResult(int resid);

    void free_results(int reqId);

    void store_vector_distribution(vector_distribution_base && dv);

    void free_vector_distribution(int reqId);

    vector_distribution_base *prepare_distr_result(int nparts);
    
    void register_result_part(int ipart, int resid, size_t size, size_t offset);

    template<class T>
    vector_distribution<T> gather_distr_parts();

    void bsp_seq_sync();

    // BSPlib
    int bsp_nprocs ();
    int bsp_pid ();
    double bsp_time ();

    void bsp_sync();

    // BSPlib MP
    void bsp_send (int pid, const void *tag, const void *payload, size_t payload_nbytes);
    void bsp_qsize (int * nmessages, size_t * accum_nbytes);
    void bsp_get_tag (int * status , void * tag);
    void bsp_move (void *payload, size_t reception_nbytes);
    void bsp_set_tagsize (size_t *tag_nbytes);

    // BSPlib DRMA
    void bsp_push_reg (const void *ident, size_t size);
    void bsp_pop_reg (const void *ident);
    void bsp_put (int pid, const void *src, void *dst, long int offset, size_t nbytes);
    void bsp_get (int pid, const void *src, long int offset, void *dst, size_t nbytes);
};


} // namespace activebsp

#include "Actor.hpp"

#endif // __ACTOR_H__
