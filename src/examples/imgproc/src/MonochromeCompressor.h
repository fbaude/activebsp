#ifndef __MONOCHROME_COMPRESSOR_H__
#define __MONOCHROME_COMPRESSOR_H__

#include "Actor.h"
#include "decl_actor.h"
#include "vector_distribution.h"

using namespace activebsp;

class MonochromeCompressor : public activebsp::ActorBase
{
private:
    vector_distribution<char> _dv;

    std::vector<char> _part;
    std::vector<char> _img;
    std::vector<char> _compressed;

    int _width;
    int _height;
    int _compress_width;
    int _compress_height;
    int _header_offset;
    bool _qraw;
    int _bitsperPixel;

    char _logpath[PATH_MAX];
    size_t _log_base_len;
    int _irequest;
    struct timespec _tfirst;
    bool _started;

    void bsp_compress();
    void bsp_compress_seq();

    typedef struct
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
    } pixel_t;

    typedef struct
    {
        unsigned char s;
    } monochrome_pixel_t;


public:
    MonochromeCompressor();

    int setLogPathBase(std::string path);

    vector_distribution<char> compress(Future<vector_distribution<char> > img, int width, int height, bool raw, int bitsPerPixel);
    vector_distribution<char> compressDv(vector_distribution<char> dv, int width, int height, bool raw, int bitsPerPixel);

    std::vector<char> compressSeq(Future<std::vector<char> > img, int width, int height, bool raw, int bitsPerPixel);

};

DECL_ACTOR(MonochromeCompressor,
          (vector_distribution<char>, compress, Future<vector_distribution<char> >, int, int, bool, int),
          (vector_distribution<char>, compressDv, vector_distribution<char>, int, int, bool, int),
          (std::vector<char>, compressSeq, Future<std::vector<char> >, int, int, bool, int),
          (int, setLogPathBase, std::string)
)

#endif // __MONOCHROME_COMPRESSOR_H__
