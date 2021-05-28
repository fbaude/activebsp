#ifndef __BOOST_SERIALIZATION_HPP__
#define __BOOST_SERIALIZATION_HPP__

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/back_inserter.hpp>

// STL serializations
#include <boost/serialization/vector.hpp>

namespace activebsp
{

class count_sink : public boost::iostreams::sink
{
private:
    std::streamsize _count = 0;

public:
    std::streamsize write(const char *, std::streamsize n)
    {
        _count += n;
        return n;
    }

    std::streamsize getCount() const { return _count; }
};

template <class Archive>
void boost_archive_op(Archive &) {}

template <class Archive, class T, class... Tn>
void boost_archive_op(Archive & ar, T & first, Tn&... others)
{
    ar & first;
    boost_archive_op(ar, others...);
}

template <class Archive, class T, class... Tn>
void boost_archive_op(Archive & ar, const T & first, const Tn&... others)
{
    ar & first;
    boost_archive_op(ar, others...);
}

template <class... Tn>
void boost_deserialize_all(char * buf, int bufsize, Tn&... args)
{
    boost::iostreams::basic_array_source<char> device(buf, bufsize);
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > is(device);
    boost::archive::binary_iarchive ia(is);

    boost_archive_op(ia, args...);
}

template <class... Tn>
std::streamsize predictBoostSerializationSize(const Tn & ... args)
{
    boost::iostreams::stream<count_sink> os {count_sink{}};
    boost::archive::binary_oarchive oa(os);

    boost_archive_op(oa, args...);

    os.flush();
    return os.component()->getCount();
}

template <class ... Tn>
void boost_serialize_all_to_buf(char * buf, int buf_size, const Tn & ... args)
{
    boost::iostreams::basic_array_sink<char> sink(buf, buf_size);
    boost::iostreams::stream<boost::iostreams::basic_array_sink<char> > os(sink);
    boost::archive::binary_oarchive oa(os);

    boost_archive_op(oa, args...);

    os.flush();
}

// This function should be generated for a number of arguments, probably inefficient with template alone

template <class Archive, class Res, class F>
Res boost_deserialize_call_archive(Archive & ar, F f)
{
    boost_archive_op(ar);

    return f();
}

template <class Archive, class Res, class F, class Arg0>
Res boost_deserialize_call_archive(Archive & ar, F f)
{
    Arg0 arg0;

    boost_archive_op(ar, arg0);

    return f(std::move(arg0));
}

template <class Archive, class Res, class F, class Arg0, class Arg1>
Res boost_deserialize_call_archive(Archive & ar, F f)
{
    Arg0 arg0;
    Arg1 arg1;

    boost_archive_op(ar, arg0, arg1);

    return f(std::move(arg0), std::move(arg1));
}

template <class Archive, class Res, class F, class Arg0, class Arg1, class Arg2>
Res boost_deserialize_call_archive(Archive & ar, F f)
{
    Arg0 arg0;
    Arg1 arg1;
    Arg2 arg2;

    boost_archive_op(ar, arg0, arg1, arg2);

    return f(std::move(arg0), std::move(arg1), std::move(arg2));
}

template <class Archive, class Res, class F, class Arg0, class Arg1, class Arg2, class Arg3>
Res boost_deserialize_call_archive(Archive & ar, F f)
{
    Arg0 arg0;
    Arg1 arg1;
    Arg2 arg2;
    Arg3 arg3;

    boost_archive_op(ar, arg0, arg1, arg2, arg3);

    return f(std::move(arg0), std::move(arg1), std::move(arg2), std::move(arg3));
}

template <class Archive, class Res, class F, class Arg0, class Arg1, class Arg2, class Arg3, class Arg4>
Res boost_deserialize_call_archive(Archive & ar, F f)
{
    Arg0 arg0;
    Arg1 arg1;
    Arg2 arg2;
    Arg3 arg3;
    Arg4 arg4;

    boost_archive_op(ar, arg0, arg1, arg2, arg3, arg4);

    return f(std::move(arg0), std::move(arg1), std::move(arg2), std::move(arg3), std::move(arg4));
}

template <class Res, class F, class... Args>
std::vector<char> boost_deserialize_call_serialize(char * buf, int bufsize, F f)
{
    // Serialization from https://stackoverflow.com/a/5604782/8015000

    boost::iostreams::basic_array_source<char> device(buf, bufsize);
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > is(device);
    boost::archive::binary_iarchive ia(is);

    Res res = boost_deserialize_call_archive<decltype(ia), Res, F, Args...>(ia, f);

    std::streamsize size = predictBoostSerializationSize(res);
    std::vector<char> serial_res;
    serial_res.reserve(size);

    boost::iostreams::back_insert_device<std::vector<char> > inserter(serial_res);
    boost::iostreams::stream<boost::iostreams::back_insert_device<std::vector<char> >> os(inserter);
    boost::archive::binary_oarchive oa(os);

    oa << res;

    // don't forget to flush the stream to finish writing into the buffer
    os.flush();

    return serial_res;
}

template <class Res>
Res boost_deserialize_result(const char * buf, int buf_size)
{
    Res ret;

    // From https://stackoverflow.com/a/5604782/8015000

    boost::iostreams::basic_array_source<char> device(buf, buf_size);
    boost::iostreams::stream<boost::iostreams::basic_array_source<char> > s(device);
    boost::archive::binary_iarchive ia(s);
    ia >> ret;

    return ret;
}

} // namespace activebsp

#endif // __BOOST_SERIALIZATION_HPP__
