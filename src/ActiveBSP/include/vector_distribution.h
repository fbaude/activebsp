#ifndef __vector_distribution_H__
#define __vector_distribution_H__

#include <memory>
#include <map>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/array_wrapper.hpp>

#include "unused.h"

namespace activebsp
{

typedef struct
{
    int pid;
    int resid;
    size_t size;
    size_t offset;
} vector_distribution_part_t;

class vector_distribution_base
{
private:
    typedef struct
    {
        int nelem;
        vector_distribution_part_t parts[];

    } vector_distribution_buf_t;

    std::vector<char> _buf;
    vector_distribution_buf_t * getStructBuf() const;

    static int size_for_elems(int);
    static bool range_overlap(int x1, int x2, int y1, int y2);

public:
    vector_distribution_base(const vector_distribution_base & other);
    //vector_distribution(const vector_distribution && other);

    vector_distribution_base(int nelems = 1);

    vector_distribution_base & operator = (const vector_distribution_base &other);
    //vector_distribution & operator = (vector_distribution && other);

    void swap(vector_distribution_base & other);

    void register_part(int i, int pid, int resid, size_t size, size_t offset);

    vector_distribution_part_t *getPart(int i);
    const vector_distribution_part_t *getPart(int i) const;

    const char *getBuf() const;
    char *getBuf();
    size_t getBufSize() const;
    int nparts() const;
    size_t getVecSize() const;

    void resize(size_t elems);

    constexpr static int parts_offset = sizeof(int);
    constexpr static int parts_size = sizeof(vector_distribution_part_t);

private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        UNUSED(version);

        size_t size = getBufSize();

        ar << size;
        ar << boost::serialization::make_array<const char>( getBuf(), size );
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        UNUSED(version);

        size_t size;
        ar >> size;
        _buf.resize(size);
        ar >> boost::serialization::make_array<char>( getBuf(), size );
    }


    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

std::ostream & operator << (std::ostream & os, const vector_distribution_base & dv);

template <class T>
class vector_distribution : public vector_distribution_base
{
  public:
    vector_distribution(const vector_distribution_base & other);
    vector_distribution(const vector_distribution<T> & other);
    vector_distribution(int nelems = 1);
    vector_distribution & operator = (const vector_distribution<T> & other);
    vector_distribution & operator = (const vector_distribution_base & other);

    size_t getVecSize() const;

  private:
    friend class boost::serialization::access;
    template<class Archive>
    void save(Archive & ar, const unsigned int version) const
    {
        UNUSED(version);

        ar << boost::serialization::base_object<vector_distribution_base>(*this);
    }
    template<class Archive>
    void load(Archive & ar, const unsigned int version)
    {
        UNUSED(version);

        ar >> boost::serialization::base_object<vector_distribution_base>(*this);
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace activebsp

#include "vector_distribution.hpp"

#endif // __vector_distribution_H__
