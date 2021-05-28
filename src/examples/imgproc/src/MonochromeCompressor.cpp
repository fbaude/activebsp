#include "MonochromeCompressor.h"

#include <fstream>
#include <stdio.h>
#include <cmath>

#include "div_range.h"
#include "measure.h"

#define LOG_MEASURES 1

MonochromeCompressor::MonochromeCompressor()
    : _log_base_len(0), _irequest(0), _started(false)
{
    bsp_push_reg(&_width, sizeof(int));
    bsp_push_reg(&_height, sizeof(int));
    bsp_push_reg(&_compress_width, sizeof(int));
    bsp_push_reg(&_compress_height, sizeof(int));
    bsp_push_reg(&_header_offset, sizeof(int));
    bsp_push_reg(&_qraw, sizeof(bool));
    bsp_push_reg(&_bitsperPixel, sizeof(int));
    bsp_push_reg(_logpath, PATH_MAX);
    bsp_push_reg(&_log_base_len, sizeof(int));
    register_spmd(&MonochromeCompressor::bsp_compress);
    register_spmd(&MonochromeCompressor::bsp_compress_seq);

    bsp_sync();
}

int MonochromeCompressor::setLogPathBase(std::string path)
{
    _log_base_len = path.size();
    strncpy(_logpath, path.c_str(), PATH_MAX - 1);

    for (int i = 0; i < bsp_nprocs(); ++i)
    {
        bsp_put(i, _logpath, _logpath, 0, _log_base_len + 1);
        bsp_put(i, &_log_base_len, &_log_base_len, 0, sizeof(int));
    }

    bsp_seq_sync();

    return 1;
}

void MonochromeCompressor::bsp_compress()
{
    int s,p;

    s = bsp_pid();
    p = bsp_nprocs();

#if LOG_MEASURES

    std::ofstream log;
    if (_log_base_len != 0)
    {
        sprintf(_logpath + _log_base_len, "_bsp_compress_p%d", bsp_pid());

        log.open(_logpath, std::ofstream::out | std::ofstream::app);
        if (!log.is_open())
        {
            std::cout << "Could not open log for writing : " << _logpath << std::endl;
        }

        if (log.tellp() == 0)
        {
            log << "#part_size\ttime_total\ttime_bcdv"
                   "\ttime_bcparams\ttime_compdims\time_resizebuf"
                   "\ttime_writeheader\ttime_getpart\ttime_bcheaderoff\ttime_compress"
                   "\ttime_register\tirequest\ttime\n";
        }
    }

    struct timespec t0,t1,t2,t3,t4,t5,t6,t7,t8,t9;

    t0 = now();
#endif

    broadcast_dv(_dv);

#if LOG_MEASURES
    t1 = now();
#endif
    if (s != 0)
    {
        bsp_get(0, &_width, 0, &_width, sizeof(int));
        bsp_get(0, &_height, 0, &_height, sizeof(int));
        bsp_get(0, &_compress_height, 0, &_compress_height, sizeof(int));
        bsp_get(0, &_compress_width, 0, &_compress_width, sizeof(int));
        bsp_get(0, &_header_offset, 0, &_header_offset, sizeof(int));
        bsp_get(0, &_qraw, 0, &_qraw, sizeof(bool));
        bsp_get(0, &_bitsperPixel, 0, &_bitsperPixel, sizeof(int));
    }

    bsp_sync();

#if LOG_MEASURES
    t2 = now();
#endif

    int compressed_pix_nrows = std::ceil ((double) _height / (double) _compress_height);
    int compressed_pix_ncols = std::ceil ((double) _width / (double) _compress_width);

    unsigned long compressed_part_y1, compressed_part_y2;
    div_range(_compress_height, s, p, &compressed_part_y1, &compressed_part_y2);

    int compressed_part_height = compressed_part_y2 - compressed_part_y1;

    int part_y1 = compressed_part_y1 * compressed_pix_nrows;
    int part_y2 = std::min(compressed_part_y2 * compressed_pix_nrows, size_t(_height));
    int part_height = part_y2 - part_y1;

    int part_offset = _width * part_y1 * sizeof(pixel_t);
    int part_size   = _width * part_height * sizeof(pixel_t);

    size_t buf_size = part_size;

    if (s == 0)
    {
        buf_size += _header_offset;
    }

#if LOG_MEASURES
    t3 = now();
#endif

    _part.resize(buf_size);
    char * data_start = &_part[0];

#if LOG_MEASURES
    t4 = now();
#endif

    int compressed_header_offset = 0;

    int maxval = (1 << _bitsperPixel) - 1;

    if (!_qraw)
    {
        if (s == 0)
        {
            compressed_header_offset = sprintf(data_start, "P5 %d %d %d\n", _compress_width, _compress_height, maxval);
            data_start += compressed_header_offset;
        }
    }

#if LOG_MEASURES
    t5 = now();
#endif

    dv_get_part(_dv, part_offset + _header_offset, data_start, part_size);

#if LOG_MEASURES
    t6 = now();
#endif

    if (!_qraw)
    {
        _header_offset = compressed_header_offset;

        if (s != 0)
        {
            bsp_get(0, &_header_offset, 0, &_header_offset, sizeof(int));
        }

        bsp_sync();
    }
    else
    {
        _header_offset = 0;
    }

#if LOG_MEASURES
    t7 = now();
#endif

    typedef monochrome_pixel_t out_pix_t;

    pixel_t * pixmap = (pixel_t *) data_start;

    out_pix_t * out_pix = (out_pix_t *) data_start;

    for (int iheight = 0; iheight < compressed_part_height; ++iheight)
    {
        for (int iwidth = 0; iwidth < _compress_width; ++iwidth, ++out_pix)
        {            
            double avg = 0.0;

            int x1 = iwidth * compressed_pix_ncols,
                x2 = std::min(x1 + compressed_pix_ncols, _width),
                y1 = iheight * compressed_pix_nrows,
                y2 = std::min(y1 + compressed_pix_nrows, part_height),
                square_npix = (y2 - y1) * (x2 - x1);

            int npix_check = 0;
            for (int y = y1; y < y2; ++y)
            {
                for (int x = x1; x < x2; ++x)
                {
                    pixel_t * pix = pixmap + y * _width + x;

                    double pix_avg = (pix->r + pix->g + pix->b) / 3;

                    avg += pix_avg / (double) square_npix;
                    ++npix_check;
                }
            }

            unsigned char pix_val = (unsigned char)( std::floor((double(avg) / 256.0) * (maxval)));

            *out_pix= {pix_val};
        }
    }

    int data_size = _compress_width * compressed_part_height * sizeof(out_pix_t);
    int compress_part_offset = compressed_part_y1 * _compress_width * sizeof(out_pix_t);

#if LOG_MEASURES
    t8 = now();
#endif
    register_single_part_result(&_part[0], s == 0 ? _header_offset + data_size : data_size, s == 0 ? compress_part_offset : _header_offset + compress_part_offset);
    bsp_sync();

#if LOG_MEASURES
    t9 = now();

    double time_total = diff_to_sec(&t0,&t9);
    double time_bcdv = diff_to_sec(&t0,&t1);
    double time_bcparams = diff_to_sec(&t1,&t2);
    double time_compdims  = diff_to_sec(&t2,&t3);
    double time_resizebuf  = diff_to_sec(&t3,&t4);
    double time_writeheader  = diff_to_sec(&t4,&t5);
    double time_getpart  = diff_to_sec(&t5,&t6);
    double time_bcheaderoff  = diff_to_sec(&t6,&t7);
    double time_compress  = diff_to_sec(&t7,&t8);
    double time_register  = diff_to_sec(&t8,&t9);
    double time_since_first = diff_to_sec(&_tfirst, &t0);

    if (log.is_open())
    {
        log << data_size << std::scientific
            << "\t" << time_total
            << "\t" << time_bcdv        << "\t" << time_bcparams
            << "\t" << time_compdims     << "\t" << time_resizebuf
            << "\t" << time_writeheader << "\t" << time_getpart
            << "\t" << time_bcheaderoff << "\t" << time_compress
            << "\t" << time_register
            << "\t" << _irequest++ << "\t" << time_since_first
            << "\n";

        log.close();
    }
#endif
}

vector_distribution<char> MonochromeCompressor::compress(Future<vector_distribution<char> > img, int width, int height, bool raw, int bitsPerPixel)
{
#if LOG_MEASURES
    if (!_started)
    {
        _tfirst = now();
        _started = true;
    }
#endif

    vector_distribution<char> img_dv = img.get();
    vector_distribution<char> dv_out = compressDv(img_dv, width, height, raw, bitsPerPixel);

    dv_release(img_dv);
    img.release();

    return dv_out;
}

vector_distribution<char> MonochromeCompressor::compressDv(vector_distribution<char> dv, int width, int height, bool raw, int bitsPerPixel)
{
    _dv = dv;

    _qraw = raw;
    _bitsperPixel = bitsPerPixel;

    char header_buf[100];

    dv_get_part(_dv, 0, header_buf, 100);
    sscanf(header_buf, "P6 %d %d 255\n", &_width, &_height);

    _header_offset = strchr(header_buf, '\n') - header_buf + 1;

    _compress_width = width;
    _compress_height = height;

    bsp_run(&MonochromeCompressor::bsp_compress);

    return gather_distr_parts<char>();
}



std::vector<char> MonochromeCompressor::compressSeq(Future<std::vector<char> > img, int width, int height, bool raw, int bitsPerPixel)
{
    _img = img.get();

    _qraw = raw;
    _bitsperPixel = bitsPerPixel;

    sscanf(&_img[0], "P6 %d %d 255\n", &_width, &_height);

    _header_offset = strchr(&_img[0], '\n') - &_img[0] + 1;

    _compress_width = width;
    _compress_height = height;

    _compressed.resize(_compress_width * _compress_height);

    bsp_run(&MonochromeCompressor::bsp_compress_seq);

    return _compressed;
}

void MonochromeCompressor::bsp_compress_seq()
{
    int s,p;

    s = bsp_pid();
    p = bsp_nprocs();

    if (s != 0)
    {
        bsp_get(0, &_width, 0, &_width, sizeof(int));
        bsp_get(0, &_height, 0, &_height, sizeof(int));
        bsp_get(0, &_compress_height, 0, &_compress_height, sizeof(int));
        bsp_get(0, &_compress_width, 0, &_compress_width, sizeof(int));
        bsp_get(0, &_header_offset, 0, &_header_offset, sizeof(int));
        bsp_get(0, &_qraw, 0, &_qraw, sizeof(bool));
        bsp_get(0, &_bitsperPixel, 0, &_bitsperPixel, sizeof(int));

        _img.resize(1);
        _compressed.resize(1);
    }

    bsp_push_reg(&_img[0], _img.size());
    bsp_push_reg(&_compressed[0], _compressed.size());

    bsp_sync();

    int compressed_pix_nrows = std::ceil ((double) _height / (double) _compress_height);
    int compressed_pix_ncols = std::ceil ((double) _width / (double) _compress_width);

    unsigned long compressed_part_y1, compressed_part_y2;
    div_range(_compress_height, s, p, &compressed_part_y1, &compressed_part_y2);

    int compressed_part_height = compressed_part_y2 - compressed_part_y1;

    int part_y1 = compressed_part_y1 * compressed_pix_nrows;
    int part_y2 = std::min(compressed_part_y2 * compressed_pix_nrows, size_t(_height));
    int part_height = part_y2 - part_y1;

    //int part_offset = _width * part_y1 * sizeof(pixel_t);
    int part_size   = _width * part_height * sizeof(pixel_t);

    size_t buf_size = part_size;

    if (s == 0)
    {
        buf_size += _header_offset;
    }

    _part.resize(buf_size);
    char * data_start = &_part[0];

    int compressed_header_offset = 0;

    int maxval = (1 << _bitsperPixel) - 1;

    if (!_qraw)
    {
        if (s == 0)
        {
            compressed_header_offset = sprintf(data_start, "P5 %d %d %d\n", _compress_width, _compress_height, maxval);
            data_start += compressed_header_offset;
        }
    }

    bsp_push_reg(&_part[0], buf_size);
    bsp_sync();

    if (s == 0)
    {
        for (int i = 0; i < bsp_nprocs(); ++i)
        {
            unsigned long i_compressed_part_y1, i_compressed_part_y2;
            div_range(_compress_height, i, p, &i_compressed_part_y1, &i_compressed_part_y2);

            int i_part_y1 = i_compressed_part_y1 * compressed_pix_nrows;
            int i_part_y2 = std::min(i_compressed_part_y2 * compressed_pix_nrows, size_t(_height));
            int i_part_height = i_part_y2 - i_part_y1;

            int i_part_offset = _width * i_part_y1 * sizeof(pixel_t);
            int i_part_size   = _width * i_part_height * sizeof(pixel_t);

            bsp_put(i, &_img[i_part_offset + _header_offset], &_part[0], 0, i_part_size);
        }
    }

    //bsp_get(0, &_img[0], part_offset + _header_offset, data_start, part_size);

    bsp_sync();

    if (!_qraw)
    {
        _header_offset = compressed_header_offset;

        if (s != 0)
        {
            bsp_get(0, &_header_offset, 0, &_header_offset, sizeof(int));
        }

        bsp_sync();
    }
    else
    {
        _header_offset = 0;
    }

    typedef monochrome_pixel_t out_pix_t;

    pixel_t * pixmap = (pixel_t *) data_start;

    out_pix_t * out_pix = (out_pix_t *) data_start;

    for (int iheight = 0; iheight < compressed_part_height; ++iheight)
    {
        for (int iwidth = 0; iwidth < _compress_width; ++iwidth, ++out_pix)
        {
            double avg = 0.0;

            int x1 = iwidth * compressed_pix_ncols,
                x2 =  x1 + compressed_pix_ncols <= _width ? x1 + compressed_pix_ncols : _width,
                y1 = iheight * compressed_pix_nrows,
                y2  = y1 + compressed_pix_nrows <= part_height ? y1 + compressed_pix_nrows : part_height,
                square_npix = (y2 - y1) * (x2 - x1);

            int npix_check = 0;
            for (int y = y1; y < y2; ++y)
            {
                for (int x = x1; x < x2; ++x)
                {
                    pixel_t * pix = pixmap + y * _width + x;

                    double pix_avg = (pix->r + pix->g + pix->b) / 3;

                    avg += pix_avg / (double) square_npix;
                    ++npix_check;
                }
            }

            unsigned char pix_val = (unsigned char)( std::floor((double(avg) / 256.0) * (maxval)));

            *out_pix= {pix_val};
        }
    }

    //int data_size = _compress_width * compressed_part_height * sizeof(out_pix_t);
    //int compress_part_offset = compressed_part_y1 * _compress_width * sizeof(out_pix_t);

    if (s == 0)
    {
        for (int i = 0; i < bsp_nprocs(); ++i)
        {
            unsigned long i_compressed_part_y1, i_compressed_part_y2;
            div_range(_compress_height, i, p, &i_compressed_part_y1, &i_compressed_part_y2);
            int i_compressed_part_height = i_compressed_part_y2 - i_compressed_part_y1;

            //int i_part_y1 = i_compressed_part_y1 * compressed_pix_nrows;
            //int i_part_y2 = std::min(i_compressed_part_y2 * compressed_pix_nrows, size_t(_height));
            //int i_part_height = i_part_y2 - i_part_y1;

            //int i_part_offset = _width * i_part_y1 * sizeof(pixel_t);
            //int i_part_size   = _width * i_part_height * sizeof(pixel_t);
            int i_data_size = _compress_width * i_compressed_part_height * sizeof(out_pix_t);
            int i_compress_part_offset = i_compressed_part_y1 * _compress_width * sizeof(out_pix_t);

            bsp_get(i, &_part[0], 0, &_compressed[i == 0 ? i_compress_part_offset : _header_offset + i_compress_part_offset],
                                                 (i == 0 ? _header_offset + i_data_size : i_data_size));
        }
    }

    //bsp_put(0, &_part[0], &_compressed[0], s == 0 ? compress_part_offset : _header_offset + compress_part_offset,
    //                                       s == 0 ? _header_offset + data_size : data_size);

    bsp_sync();

    bsp_pop_reg(&_img[0]);
    bsp_pop_reg(&_compressed[0]);
}

REGISTER_ACTOR(MonochromeCompressor)
