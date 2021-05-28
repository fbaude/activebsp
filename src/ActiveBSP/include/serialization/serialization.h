#ifndef __SERIALIZATION_H__
#define __SERIALIZATION_H__

/*

#include <string>
#include <vector>

namespace activebsp
{

template <class Archive>
void serialize_all(Archive & ar);

template <class Archive, class T, class... Tn>
void serialize_all(Archive & ar, const T & first, const Tn&... others);

template <class Archive>
void deserialize_all(Archive & ar);

template <class Archive, class T, class... Tn>
void deserialize_all(Archive & ar, T & first, Tn&... others);

// This function should be generated for a number of arguments
template <class Archive, class Res, class F>
Res deserialize_call(Archive & ar, F f);

template <class Archive, class Res, class F, class Arg0>
Res deserialize_call(Archive & ar, F f);

template <class Archive, class Res, class F, class Arg0, class Arg1>
Res deserialize_call(Archive & ar, F f);

template <class Archive, class Res, class F, class Arg0, class Arg1, class Arg2>
Res deserialize_call(Archive & ar, F f);

template <class Archive, class Res, class F, class Arg0, class Arg1, class Arg2, class Arg3>
Res deserialize_call(Archive & ar, F f);

template <class Archive, class Res, class F, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
Res deserialize_call(Archive & ar, F f);


template <class Res, class F, class... Args>
std::string deserialize_call_serialize(int src, F f);

template <class Res>
Res deserialize_result(const char * buf, int buf_size);

template <class Res>
Res deserialize_result(const std::vector<char> & v);

}


*/

#include "serialization.hpp"

#endif // __SERIALIZATION_H__
