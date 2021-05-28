#ifndef __CONTIGUOUS_HPP__
#define __CONTIGUOUS_HPP__

#include <type_traits>
#include <vector>

namespace activebsp
{

template <class T>
struct is_vector : std::false_type
{};

template <class T>
struct is_vector<std::vector<T> > : public std::true_type
{};

template <class T>
struct is_contiguous_vector : public std::false_type
{};

template <class T>
struct is_contiguous_vector<std::vector<T> > : public std::is_pod<T>
{};

template <class T>
struct is_contiguous : std::integral_constant<bool, std::is_pod<T>::value || is_contiguous_vector<T>::value >
{};

template <class T = void, class ... Tn>
struct are_contiguous
        : public std::integral_constant<bool, is_contiguous<T>::value && are_contiguous<Tn...>::value >
{};

template <>
struct are_contiguous<void> : public std::true_type
{};


} // namespace activebsp

#endif // __CONTIGUOUS_HPP__
