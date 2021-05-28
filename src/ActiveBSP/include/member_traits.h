#ifndef __MEMBER_TRAITS_H__
#define __MEMBER_TRAITS_H__

namespace activebsp
{

template <class T, class Ret, class ... Args>
struct member_function_type
{
    typedef Ret(T::*type)(Args...);
};

//TODO generate for more arg number

template <class T, class Ret>
inline std::function <Ret()> func_wrapper(T * obj, typename member_function_type<T, Ret>::type func)
{
    return std::bind(func, obj);
}

template <class T, class Ret, class Arg0>
inline std::function <Ret(Arg0)> func_wrapper(T * obj, typename member_function_type<T, Ret, Arg0>::type func)
{
    return std::bind(func, obj, std::placeholders::_1);
}

template <class T, class Ret, class Arg0, class Arg1>
inline std::function <Ret(Arg0, Arg1)> func_wrapper(T * obj, typename member_function_type<T, Ret, Arg0, Arg1>::type func)
{
    return std::bind(func, obj, std::placeholders::_1, std::placeholders::_2);
}

template <class T, class Ret, class Arg0, class Arg1, class Arg2>
inline std::function <Ret(Arg0, Arg1, Arg2)> func_wrapper(T * obj, typename member_function_type<T, Ret, Arg0, Arg1, Arg2>::type func)
{
    return std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
}

template <class T, class Ret, class Arg0, class Arg1, class Arg2, class Arg3>
inline std::function <Ret(Arg0, Arg1, Arg2, Arg3)> func_wrapper(T * obj, typename member_function_type<T, Ret, Arg0, Arg1, Arg2, Arg3>::type func)
{
    return std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
}

template <class T, class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
inline std::function <Ret(Arg0, Arg1, Arg2, Arg3, Arg4)> func_wrapper(T * obj, typename member_function_type<T, Ret, Arg0, Arg1, Arg2, Arg3, Arg4>::type func)
{
    return std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
}

template <class T, class Ret, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4, class Arg5>
inline std::function <Ret(Arg0, Arg1, Arg2, Arg3, Arg4, Arg5)> func_wrapper(T * obj, typename member_function_type<T, Ret, Arg0, Arg1, Arg2, Arg3, Arg4, Arg5>::type func)
{
    return std::bind(func, obj, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6);
}

} // namespace activebsp

#endif // __MEMBER_TRAITS_H__
