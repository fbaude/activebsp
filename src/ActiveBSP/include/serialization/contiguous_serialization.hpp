#ifndef __CONTIGUOUS_SERIALIZATION_HPP__
#define __CONTIGUOUS_SERIALIZATION_HPP__

#include <vector>

#include <string.h>

#include "unused.h"

namespace activebsp
{

template <class T>
size_t predict_contiguous_unit_serialization_size(const T & obj)
{
    UNUSED(obj);
    return sizeof(T);
}

template <class T>
size_t predict_contiguous_unit_serialization_size(const std::vector<T> & vec)
{
    return sizeof(size_t) + vec.size() * sizeof(T);
}


template <class T = void>
size_t predict_contiguous_serialization_size()
{
    return 0;
}

template <class T, class... Tn>
size_t predict_contiguous_serialization_size(const T & first, const Tn & ... others)
{
    return predict_contiguous_unit_serialization_size(first) + predict_contiguous_serialization_size(others...);
}

template <class T>
size_t contiguous_serialize_unit_to_buf(char * buf, int buf_size, const T & arg)
{
    UNUSED(buf_size);

    (*(T *) buf) = arg;

    return sizeof(T);
}

template <class T>
size_t contiguous_serialize_unit_to_buf(char * buf, int buf_size, const std::vector<T> & vec)
{
    UNUSED(buf_size);

    size_t size = vec.size();
    memcpy(buf, &size, sizeof(size_t));
    memcpy(buf + sizeof(size_t), &vec[0], size * sizeof(T));

    return size * sizeof(T) + sizeof(size_t);
}

template <class T = void>
void contiguous_serialize_all_to_buf(char * buf, int buf_size)
{
    UNUSED(buf);
    UNUSED(buf_size);
}

template <class T, class ... Tn>
void contiguous_serialize_all_to_buf(char * buf, int buf_size, const T & first, const Tn & ... others)
{
    size_t offset = contiguous_serialize_unit_to_buf(buf, buf_size, first);
    contiguous_serialize_all_to_buf(buf + offset, buf_size - offset, others...);
}

template <class T>
size_t contiguous_deserialize_unit(char * buf, int buf_size, T & arg)
{
    UNUSED(buf_size);

    int size = predict_contiguous_unit_serialization_size(arg);
    arg = (*(T *) buf);

    return size;
}


template <class T>
size_t contiguous_deserialize_unit(char * buf, int buf_size, std::vector<T> & vec)
{
    size_t size;
    memcpy(&size, buf, sizeof(size_t));

    buf += sizeof(size_t);
    buf_size -= sizeof(size_t);

    vec.resize(size);
    memcpy(&vec[0], buf, size * sizeof(T));
    //vec.assign((T *) buf, (T *) (buf + buf_size));

    return size * sizeof(T) + sizeof(size_t);
}

template <class T = void>
void contiguous_deserialize_all(char * buf, int buf_size)
{
    UNUSED(buf);
    UNUSED(buf_size);
}

template <class T, class... Tn>
void contiguous_deserialize_all(char * buf, int buf_size, T & first, Tn&... others)
{
    size_t offset = contiguous_deserialize_unit(buf, buf_size, first);
    contiguous_deserialize_all(buf + offset, buf_size - offset, others...);
}

} // namespace activebsp

#endif // __CONTIGUOUS_SERIALIZATION_HPP__
