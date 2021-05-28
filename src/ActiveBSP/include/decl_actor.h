#ifndef DECL_ACTOR_H
#define DECL_ACTOR_H

#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <string.h>

#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor/expand.hpp>
#include <boost/preprocessor/tuple/push_front.hpp>

#include <mpi.h>

#include "management/MasterProxy.h"
#include "Proxy.h"
#include "Future.h"
#include "worker/ActorWorker.h"
#include "serialization/serialization.h"
#include "activebsp.h"
#include "ActiveBSPlib.h"
#include "SyntaxHelper.h"
#include "member_traits.h"

namespace activebsp
{

template <class T, class Ret, class... Args>
inline std::vector<char> actor_function_handler(char * argsBuf, int argsSize, ActorBase * obj, typename member_function_type<T, Ret, Args...>::type func)
{
    auto f_wrapper = func_wrapper<T, Ret, Args...>((T *) obj, func);

    return activebsp::deserialize_call_serialize<Ret, decltype(f_wrapper), Args...> (argsBuf, argsSize, f_wrapper);
}

}

#define GET_REGISTER_FUNC_HANDLER_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,NAME,...) NAME
#define REGISTER_FUNC_HANDLER(...) GET_REGISTER_FUNC_HANDLER_MACRO(__VA_ARGS__, \
    REGISTER_FUNC_HANDLER_6, REGISTER_FUNC_HANDLER_5, REGISTER_FUNC_HANDLER_4, REGISTER_FUNC_HANDLER_3, REGISTER_FUNC_HANDLER_2, REGISTER_FUNC_HANDLER_1, REGISTER_FUNC_HANDLER_0)(__VA_ARGS__)

#define REGISTER_FUNC_HANDLER_0(CLASS, RET, NAME) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \

#define REGISTER_FUNC_HANDLER_1(CLASS, RET, NAME, A0) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET, A0>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \

#define REGISTER_FUNC_HANDLER_2(CLASS, RET, NAME, A0, A1) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET, A0, A1>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \

#define REGISTER_FUNC_HANDLER_3(CLASS, RET, NAME, A0, A1, A2) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET, A0, A1, A2>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \

#define REGISTER_FUNC_HANDLER_4(CLASS, RET, NAME, A0, A1, A2, A3) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET, A0, A1, A2, A3>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \

#define REGISTER_FUNC_HANDLER_5(CLASS, RET, NAME, A0, A1, A2, A3, A4) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET, A0, A1, A2, A3, A4>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \

#define REGISTER_FUNC_HANDLER_6(CLASS, RET, NAME, A0, A1, A2, A3, A4, A5) \
worker.register_function(#NAME, std::bind(actor_function_handler<CLASS, RET, A0, A1, A2, A3, A4, A5>, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, &CLASS::NAME)); \


#define GET_DECLARE_PROXY_FUNC_MACRO(_1,_2,_3,_4,_5,_6,_7,NAME,...) NAME
#define DECLARE_PROXY_FUNC(...) GET_DECLARE_PROXY_FUNC_MACRO(__VA_ARGS__, \
    DECLARE_PROXY_FUNC_5, DECLARE_PROXY_FUNC_4, DECLARE_PROXY_FUNC_3, DECLARE_PROXY_FUNC_2, DECLARE_PROXY_FUNC_1, DECLARE_PROXY_FUNC_0)(__VA_ARGS__)

#define DECLARE_PROXY_FUNC_0(RET, NAME) \
Future <RET> NAME () \
{ \
    return active_call <RET> (#NAME); \
}

#define DECLARE_PROXY_FUNC_1(RET, NAME, A0) \
Future <RET> NAME (A0 a0) \
{ \
    return active_call <RET, A0> (#NAME, a0); \
}

#define DECLARE_PROXY_FUNC_2(RET, NAME, A0, A1) \
Future <RET> NAME (A0 a0, A1 a1) \
{ \
    return active_call <RET, A0, A1> (#NAME, a0, a1); \
}

#define DECLARE_PROXY_FUNC_3(RET, NAME, A0, A1, A2) \
Future <RET> NAME (A0 a0, A1 a1, A2 a2) \
{ \
    return active_call <RET, A0, A1, A2> (#NAME, a0, a1, a2); \
}

#define DECLARE_PROXY_FUNC_4(RET, NAME, A0, A1, A2, A3) \
Future <RET> NAME (A0 a0, A1 a1, A2 a2, A3 a3) \
{ \
    return active_call <RET, A0, A1, A2, A3> (#NAME, a0, a1, a2, a3); \
}

#define DECLARE_PROXY_FUNC_5(RET, NAME, A0, A1, A2, A3, A4) \
Future <RET> NAME (A0 a0, A1 a1, A2 a2, A3 a3, A4 a4) \
{ \
    return active_call <RET, A0, A1, A2, A3, A4> (#NAME, a0, a1, a2, a3, a4); \
}

#define GET_DECLARE_PROXY_FUNCS_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10, NAME,...) NAME
#define DECLARE_PROXY_FUNCS(...) GET_DECLARE_PROXY_FUNCS_MACRO(__VA_ARGS__, \
    DECLARE_PROXY_FUNCS_10, DECLARE_PROXY_FUNCS_9, DECLARE_PROXY_FUNCS_8, \
    DECLARE_PROXY_FUNCS_7, DECLARE_PROXY_FUNCS_6, DECLARE_PROXY_FUNCS_5, DECLARE_PROXY_FUNCS_4, \
    DECLARE_PROXY_FUNCS_3, DECLARE_PROXY_FUNCS_2, DECLARE_PROXY_FUNCS_1)(__VA_ARGS__)

#define DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNC F0

#define DECLARE_PROXY_FUNCS_1(F0) \
DECLARE_PROXY_FUNCS_ITEM(F0)

#define DECLARE_PROXY_FUNCS_2(F0, F1) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_1(F1)

#define DECLARE_PROXY_FUNCS_3(F0, F1, F2) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_2(F1, F2)

#define DECLARE_PROXY_FUNCS_4(F0, F1, F2, F3) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_3(F1, F2, F3)

#define DECLARE_PROXY_FUNCS_5(F0, F1, F2, F3, F4) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_4(F1, F2, F3, F4)

#define DECLARE_PROXY_FUNCS_6(F0, F1, F2, F3, F4, F5) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_5(F1, F2, F3, F4, F5)

#define DECLARE_PROXY_FUNCS_7(F0, F1, F2, F3, F4, F5, F6) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_6(F1, F2, F3, F4, F5, F6)

#define DECLARE_PROXY_FUNCS_8(F0, F1, F2, F3, F4, F5, F6, F7) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_7(F1, F2, F3, F4, F5, F6, F7)

#define DECLARE_PROXY_FUNCS_9(F0, F1, F2, F3, F4, F5, F6, F7, F8) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_8(F1, F2, F3, F4, F5, F6, F7, F8)

#define DECLARE_PROXY_FUNCS_10(F0, F1, F2, F3, F4, F5, F6, F7, F8, F9) \
DECLARE_PROXY_FUNCS_ITEM(F0) \
DECLARE_PROXY_FUNCS_9(F1, F2, F3, F4, F5, F6, F7, F8, F9)


#define GET_REGISTER_FUNC_HANDLERS_MACRO(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,NAME,...) NAME
#define REGISTER_FUNC_HANDLERS(...) GET_REGISTER_FUNC_HANDLERS_MACRO(__VA_ARGS__, \
    REGISTER_FUNC_HANDLERS_10, REGISTER_FUNC_HANDLERS_9, \
    REGISTER_FUNC_HANDLERS_8, REGISTER_FUNC_HANDLERS_7, REGISTER_FUNC_HANDLERS_6, REGISTER_FUNC_HANDLERS_5, \
    REGISTER_FUNC_HANDLERS_4, REGISTER_FUNC_HANDLERS_3, REGISTER_FUNC_HANDLERS_2, REGISTER_FUNC_HANDLERS_1)(__VA_ARGS__)

#define REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
BOOST_PP_EXPAND(REGISTER_FUNC_HANDLER BOOST_PP_TUPLE_PUSH_FRONT(F0, CLASS)) \

#define REGISTER_FUNC_HANDLERS_1(CLASS, F0) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS,F0) \

#define REGISTER_FUNC_HANDLERS_2(CLASS, F0, F1) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_1(CLASS, F1)

#define REGISTER_FUNC_HANDLERS_3(CLASS, F0, F1, F2) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_2(CLASS, F1, F2)

#define REGISTER_FUNC_HANDLERS_4(CLASS, F0, F1, F2, F3) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_3(CLASS, F1, F2, F3)

#define REGISTER_FUNC_HANDLERS_5(CLASS, F0, F1, F2, F3, F4) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_4(CLASS, F1, F2, F3, F4)

#define REGISTER_FUNC_HANDLERS_6(CLASS, F0, F1, F2, F3, F4, F5) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_5(CLASS, F1, F2, F3, F4, F5)

#define REGISTER_FUNC_HANDLERS_7(CLASS, F0, F1, F2, F3, F4, F5, F6) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_6(CLASS, F1, F2, F3, F4, F5, F6)

#define REGISTER_FUNC_HANDLERS_8(CLASS, F0, F1, F2, F3, F4, F5, F6, F7) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_7(CLASS, F1, F2, F3, F4, F5, F6, F7)

#define REGISTER_FUNC_HANDLERS_9(CLASS, F0, F1, F2, F3, F4, F5, F6, F7, F8) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_8(CLASS, F1, F2, F3, F4, F5, F6, F7, F8)

#define REGISTER_FUNC_HANDLERS_10(CLASS, F0, F1, F2, F3, F4, F5, F6, F7, F8, F9) \
REGISTER_FUNC_HANDLERS_ITEM(CLASS, F0) \
REGISTER_FUNC_HANDLERS_9(CLASS, F1, F2, F3, F4, F5, F6, F7, F8, F9)

#define DECL_MAKER(CLASS) \
class CLASS ## Maker : public activebsp::ActorMaker \
{ \
private : \
    static CLASS ## Maker _instance; \
 \
public : \
    CLASS ## Maker() : ActorMaker(#CLASS, CLASS ## _handler) {} \
};

#define REGISTER_ACTOR(CLASS) \
namespace activebsp \
{ \
    CLASS ## Maker CLASS ## Maker::_instance; \
}

#define DECL_ACTOR(CLASS, ...) \
namespace activebsp \
{ \
 \
inline \
void CLASS ## _handler(const std::shared_ptr<IntraActorCommunicator> & intraActorCommunicator, \
                       const std::shared_ptr<MasterProxy>            & masterProxy, \
                       BSPlib * bsplib) \
{ \
    SyntaxHelper::getInstance()->setIntraActorComm(intraActorCommunicator); \
    SyntaxHelper::getInstance()->setActorBSPlib(bsplib); \
    CLASS * obj = new CLASS(); \
    obj->setMasterProxy(masterProxy); \
    ActorWorker worker(obj, SyntaxHelper::getInstance()->getComm(), intraActorCommunicator, masterProxy); \
 \
    REGISTER_FUNC_HANDLERS(CLASS,__VA_ARGS__)\
 \
    worker.handler(); \
} \
 \
template <> \
class Proxy<CLASS> : public ProxyBase \
{ \
private: \
    friend class boost::serialization::access; \
    template<class Archive> \
    void serialize(Archive &ar, const unsigned int) \
    { \
        ar & boost::serialization::base_object<ProxyBase>(*this); \
    }    \
\
public: \
    Proxy(const std::shared_ptr<ActorCommunicator> & comm, int dst) : ProxyBase(comm, dst) {} \
    Proxy() {} \
 \
    DECLARE_PROXY_FUNCS(__VA_ARGS__) \
}; \
 \
template <> inline \
Proxy <CLASS> createActiveObject<CLASS>(const std::vector <int> & pids) \
{ \
    SyntaxHelper::getInstance()->getComm()->askCreateActor(pids, #CLASS); \
 \
    return Proxy<CLASS>(SyntaxHelper::getInstance()->getComm(), pids[0]); \
} \
 \
    DECL_MAKER(CLASS) \
 \
} // namespace activebsp


#endif // DECL_ACTOR_H
