#include "Actor.h"

#include <iostream>
#include <algorithm>
#include <utility>

#include <string.h>

#include "SyntaxHelper.h"
#include "actormsg/GetResultPartsMessage.h"
#include "actormsg/StoreResultMessage.h"
#include "log.h"

#include "future/FutureKeyManager.h"

#define BROADCASTDV_OPT 0

namespace activebsp
{

ActorBase::ActorBase()
{
    _bsplib         = SyntaxHelper::getInstance()->getActorBSPlib();
    _actorComm      = SyntaxHelper::getInstance()->getComm();
    _intraActorComm = SyntaxHelper::getInstance()->getIntraActorComm();

    register_spmd(&ActorBase::bsp_sync);

    _next_spmd_function_id = -1;
    bsp_push_reg(&_next_spmd_function_id, sizeof(int));

    _dv_buf_nelems = 0;
    bsp_push_reg(&_dv_buf_nelems,    sizeof(int));

    bsp_sync();
}

void ActorBase::setMasterProxy(const std::shared_ptr<MasterProxy> & masterProxy)
{
    _masterProxy = masterProxy;
}


void ActorBase::call_spmd(int function_id)
{
    if (_spmd_functions.size())
    {
        _spmd_functions[function_id]();
    }
    else
    {
        std::cerr << "Received unknown slave message : " << function_id << std::endl;
    }

}

void ActorBase::kill_slaves()
{
    int destroy_id = 0;
    _intraActorComm->sendSPMDFunctionID(destroy_id);
}

bool ActorBase::is_master()
{
    return bsp_pid() == 0;
}

void ActorBase::setRequestId(int reqId) { _reqId = reqId; }
int ActorBase::getRequestId() const { return _reqId; }

int ActorBase::getNPendingRequests()
{
    return _masterProxy->getNPendingRequests();
}

void ActorBase::run_function_id(int id)
{
    int send_id = id + 1;

    _intraActorComm->sendSPMDFunctionID(send_id);

    _spmd_functions[id]();
}

vector_distribution_base * ActorBase::prepare_distr_result(int nparts)
{
    _cur_vector_distribution = vector_distribution_base(nparts);
    _cur_vector_distribution_part_size = 0;
    bsp_push_reg(_cur_vector_distribution.getBuf(), _cur_vector_distribution.getBufSize());
    bsp_sync();

    return &_cur_vector_distribution;
}

void ActorBase::register_result_part(int ipart, int resid, size_t size, size_t offset)
{
    _cur_vector_distribution.register_part(ipart, _actorComm->pid(), resid, size, offset);
}

void ActorBase::store_vector_distribution(vector_distribution_base && dv)
{
    _RequestDistrVectors.emplace(std::make_pair(_reqId, dv));
}

void ActorBase::free_vector_distribution(int reqId)
{
    _RequestDistrVectors.erase(reqId);
}

int ActorBase::store_result(const char * buf, size_t size)
{
    LOG_TRACE("Storing result of size %zu", size);

    int key = FutureKeyManager::getInstance()->takeKey();

    START_MEASURE(1);
    _actorComm->store_result(key, buf, size);
    END_MEASURE(1);
    LOG_MEASURE(1, "Storing result");

    return key;
}

void ActorBase::register_single_part_result(const char * data, size_t size, size_t offset)
{
    int resid = store_result(data, size);
    prepare_distr_result(1);
    register_result_part(0, resid, size, offset);
}

std::vector<char> ActorBase::get_part(const vector_distribution_base & dv, size_t offset, size_t size)
{
    std::vector<char> vec(size);
    get_part(dv, offset, &vec[0], size);

    return vec;
}

void ActorBase::broadcast_dv(vector_distribution_base & dv)
{
    int dv_buf_nelems_old = _dv_buf_nelems;
    int dv_buf_new = dv.nparts();

    if (bsp_pid() == 0)
    {
        if (BROADCASTDV_OPT && dv_buf_nelems_old == dv_buf_new && dv_buf_nelems_old != 0)
        {
            for (int i = 0; i < bsp_nprocs(); ++i)
            {
                bsp_put(i, dv.getBuf(), &_dv_buf[0], 0, dv.getBufSize());
            }
        }
        else
        {
            for (int i = 0; i < bsp_nprocs(); ++i)
            {
                bsp_put(i, &dv_buf_new, &_dv_buf_nelems, 0, sizeof(int));
            }
        }
    }

    bsp_sync();

    if (BROADCASTDV_OPT && dv_buf_nelems_old == _dv_buf_nelems && dv_buf_nelems_old != 0)
    {
        if (bsp_pid() != 0)
        {
            dv = vector_distribution_base(_dv_buf_nelems);
            memcpy(&_dv_buf[0], dv.getBuf(), dv.getBufSize());
        }
    }
    else
    {
        if (bsp_pid() != 0)
        {
            dv = vector_distribution_base(_dv_buf_nelems);
        }

        if (_dv_buf.size() < dv.getBufSize())
        {
            if (_dv_buf.size() != 0)
            {
                bsp_pop_reg(&_dv_buf[0]);
            }

            _dv_buf.resize(dv.getBufSize());
            bsp_push_reg(&_dv_buf[0], dv.getBufSize());

            bsp_sync();
        }

        if (bsp_pid() == 0)
        {
            memcpy(&_dv_buf[0], dv.getBuf(), dv.getBufSize());
        }
        else
        {
            bsp_get(0, &_dv_buf[0], 0, dv.getBuf(), dv.getBufSize());
        }

        bsp_sync();
    }
}

void ActorBase::get_part(const vector_distribution_base & dv, size_t offset, char * out_buf, size_t size)
{
    _actorComm->dv_get_part(dv, offset, out_buf, size);
}

void ActorBase::bsp_seq_sync()
{
    bsp_run(&ActorBase::bsp_sync);
}

int ActorBase::bsp_nprocs ()
{
    return _bsplib->nprocs();
}

int ActorBase::bsp_pid ()
{
    return _bsplib->pid();
}

double ActorBase::bsp_time ()
{
    return _bsplib->time();
}

void ActorBase::bsp_sync()
{
    _bsplib->sync();
}

void ActorBase::bsp_send (int pid, const void *tag, const void *payload, size_t payload_nbytes)
{
    _bsplib->send(pid, tag, payload, payload_nbytes);
}

void ActorBase::bsp_qsize (int * nmessages, size_t * accum_nbytes)
{
    _bsplib->qsize(nmessages, accum_nbytes);
}

void ActorBase::bsp_get_tag (int * status , void * tag)
{
    _bsplib->get_tag(status, tag);
}

void ActorBase::bsp_move (void *payload, size_t reception_nbytes)
{
    _bsplib->move(payload, reception_nbytes);
}

void ActorBase::bsp_set_tagsize (size_t *tag_nbytes)
{
    _bsplib->set_tagsize(tag_nbytes);
}

void ActorBase::bsp_push_reg (const void *ident, size_t size)
{
    _bsplib->push_reg(ident, size);
}

void ActorBase::bsp_pop_reg (const void *ident)
{
    _bsplib->pop_reg(ident);
}

void ActorBase::bsp_put (int pid, const void *src, void *dst, long int offset, size_t nbytes)
{
    _bsplib->put(pid, src, dst, offset, nbytes);
}

void ActorBase::bsp_get (int pid, const void *src, long int offset, void *dst, size_t nbytes)
{
    _bsplib->get(pid, src, offset, dst, nbytes);
}


} // namespace activebsp
