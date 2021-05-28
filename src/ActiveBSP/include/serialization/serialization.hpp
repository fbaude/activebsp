#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <type_traits>

#include "serialization/boost_serialization.hpp"
#include "serialization/contiguous_serialization.hpp"
#include "serialization/contiguous.hpp"

namespace activebsp
{

template <class... Tn,
          typename std::enable_if<are_contiguous<Tn...>::value>::type * = nullptr>
size_t predictSerializationSize(const Tn & ... args)
{
    return predict_contiguous_serialization_size(args...);
}

template <class... Tn,
          typename std::enable_if<!are_contiguous<Tn...>::value>::type * = nullptr>
size_t predictSerializationSize(const Tn & ... args)
{
    return (size_t) predictBoostSerializationSize(args...);
}



template <class ... Tn,
          typename std::enable_if<are_contiguous<Tn...>::value>::type * = nullptr>
void serialize_all_to_buf(char * buf, int buf_size, const Tn & ... args)
{
    contiguous_serialize_all_to_buf(buf, buf_size, args...);
}

template <class ... Tn,
          typename std::enable_if<!are_contiguous<Tn...>::value>::type * = nullptr>
void serialize_all_to_buf(char * buf, int buf_size, const Tn & ... args)
{
    boost_serialize_all_to_buf(buf, buf_size, args...);
}

template <class ... Tn>
void serialize_all_to_vchar_ref(std::vector<char> & serial, const Tn & ... args)
{
    std::streamsize size = predictSerializationSize(args...);
    serial.resize(size);

    serialize_all_to_buf(&serial[0], serial.size(), args...);
}


template <class... Tn>
std::vector<char> serialize_all_to_vchar(const Tn & ... args)
{
    size_t size = predictSerializationSize(args...);
    std::vector<char> serial;
    serial.resize(size);

    serialize_all_to_buf(&serial[0], size, args...);

    return serial;
}



template <class... Tn,
          typename std::enable_if<are_contiguous<Tn...>::value>::type * = nullptr>
void deserialize_all(char * buf, int bufsize, Tn&... args)
{
    contiguous_deserialize_all(buf, bufsize, args...);
}

template <class... Tn,
          typename std::enable_if<!are_contiguous<Tn...>::value>::type * = nullptr>
void deserialize_all(char * buf, int bufsize, Tn&... args)
{
    boost_deserialize_all(buf, bufsize, args...);
}



template <class Res, class F>
Res deserialize_call(char * buf, int bufsize, F f)
{
    UNUSED(buf);
    UNUSED(bufsize);

    return f();
}

template <class Res, class F, class Arg0>
Res deserialize_call(char * buf, int bufsize, F f)
{
    Arg0 arg0;

    deserialize_all(buf, bufsize, arg0);
    return f(std::move(arg0));
}

template <class Res, class F, class Arg0, class Arg1>
Res deserialize_call(char * buf, int bufsize, F f)
{
    Arg0 arg0;
    Arg1 arg1;

    deserialize_all(buf, bufsize, arg0, arg1);

    return f(std::move(arg0), std::move(arg1));
}

template <class Res, class F, class Arg0, class Arg1, class Arg2>
Res deserialize_call(char * buf, int bufsize, F f)
{
    Arg0 arg0;
    Arg1 arg1;
    Arg2 arg2;

    deserialize_all(buf, bufsize, arg0, arg1, arg2);

    return f(std::move(arg0), std::move(arg1), std::move(arg2));
}

template <class Res, class F, class Arg0, class Arg1, class Arg2, class Arg3>
Res deserialize_call(char * buf, int bufsize, F f)
{
    Arg0 arg0;
    Arg1 arg1;
    Arg2 arg2;
    Arg3 arg3;

    deserialize_all(buf, bufsize, arg0, arg1, arg2, arg3);

    return f(std::move(arg0), std::move(arg1), std::move(arg2), std::move(arg3));
}

template <class Res, class F, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
Res deserialize_call(char * buf, int bufsize, F f)
{
    Arg0 arg0;
    Arg1 arg1;
    Arg2 arg2;
    Arg3 arg3;
    Arg4 arg4;

    deserialize_all(buf, bufsize, arg0, arg1, arg2, arg3, arg4);

    return f(std::move(arg0), std::move(arg1), std::move(arg2), std::move(arg3), std::move(arg4));
}



template <class Res, class F, class... Args>
std::vector<char> deserialize_call_serialize(char * buf, int bufsize, F f)
{
    Res res = deserialize_call<Res, F, Args...>(buf, bufsize, f);

    std::vector<char> serial_res = serialize_all_to_vchar(res);

    return serial_res;
}


} // namespace activebsp

#endif // __SERIALIZATION_HPP__
