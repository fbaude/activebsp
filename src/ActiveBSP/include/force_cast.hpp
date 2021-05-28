#ifndef __FORCE_CAST_HPP__
#define __FORCE_CAST_HPP__

template<typename OUT, typename IN>
OUT force_cast( IN in )
{
    union
    {
        IN  in;
        OUT out;
    }
    u = { in };

    return u.out;
};

#endif // __FORCE_CAST_HPP__
