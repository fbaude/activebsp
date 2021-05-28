#include "vector_distribution.h"

namespace activebsp
{

template <class T>
vector_distribution<T>::vector_distribution(int nelems /* = 1 */)
    : vector_distribution_base(nelems) {}
 
template <class T>
vector_distribution<T>::vector_distribution(const vector_distribution<T> & other)
    : vector_distribution_base(other) {}

template <class T>
vector_distribution<T>::vector_distribution(const vector_distribution_base & other)
    : vector_distribution_base(other) {}

template <class T>
vector_distribution<T> & vector_distribution<T>::operator = (const vector_distribution<T> & other)
{
    vector_distribution<T> temp(other);
    swap(temp);

    return *this;
}

template <class T>
vector_distribution<T> & vector_distribution<T>::operator = (const vector_distribution_base & other)
{
    vector_distribution<T> temp(other);
    swap(temp);

    return *this;
}

template <class T>
size_t vector_distribution<T>::getVecSize() const
{
    return vector_distribution_base::getVecSize() / sizeof(T);
}

} // namespace activebsp
