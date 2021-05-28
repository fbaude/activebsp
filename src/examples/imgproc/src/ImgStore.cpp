#include "ImgStore.h"

#include <fstream>

#include <stdio.h>
#include <dirent.h>

#include "div_range.h"
#include "measure.h"

#define LOG_MEASURES 1

void ImgStore::bsp_store_img()
{
    std::ofstream log;
    if (_log_base_len != 0)
    {
        sprintf(_logpath + _log_base_len, "_bsp_store_img_p%d", bsp_pid());

        log.open(_logpath, std::ofstream::out | std::ofstream::app);
        if (!log.is_open())
        {
            std::cout << "Could not open log for writing : " << _logpath << std::endl;
        }

        if (log.tellp() == 0)
        {
            log << "#size\ttime_total\ttime_broadcast_dv\ttime_getpath\ttime_getpart\ttime_resizebuf\ttime_write\ttime_sync\n";
        }
    }
#if LOG_MEASURES
    timespec t0,t1,t2,t3,t4,t5,t6;

    t0 = now();
#endif

    broadcast_dv(_dv);

#if LOG_MEASURES
    t1 = now();
#endif

    if (bsp_pid() != 0)
    {
        bsp_get(0, &_dst[0], 0, &_dst[0], PATH_MAX);
    }

    bsp_sync();

#if LOG_MEASURES
    t2 = now();
#endif

    size_t vec_size = _dv.getVecSize();

    size_t part_offset,part_size, end;
    div_range(vec_size, bsp_pid(), bsp_nprocs(), &part_offset,&end);
    part_size = end - part_offset;

//    size_t block_size = ceil((double) vec_size / (double) bsp_nprocs());
//    size_t part_offset = bsp_pid() * block_size;
//    size_t part_size = (part_offset + block_size <= vec_size ? block_size : vec_size - part_offset);

    _part.resize(part_size);

#if LOG_MEASURES
    t3 = now();
#endif

    get_part(_dv, part_offset, &_part[0], part_size);

#if LOG_MEASURES
    t4 = now();
#endif

    size_t dst_base_len = strlen(&_dst[0]);
    sprintf(&_dst[dst_base_len], "_%d", bsp_pid());

    std::ofstream os(&_dst[0]);
//    std::cout << "Writing to path : " << &_dst[0] << std::endl;

    os.write(&_part[0], part_size);

    os.close();

#if LOG_MEASURES
    t5 = now();
#endif

    bsp_sync();

#if LOG_MEASURES
    t6 = now();

    double time_total = diff_to_sec(&t0,&t6);
    double time_broadcastdv = diff_to_sec(&t0,&t1);
    double time_getpath = diff_to_sec(&t1,&t2);
    double ttime_getpart = diff_to_sec(&t2,&t3);
    double time_resizebuf  = diff_to_sec(&t3,&t4);
    double time_write  = diff_to_sec(&t4,&t5);
    double time_sync  = diff_to_sec(&t5,&t6);

    if (log.is_open())
    {
        log << vec_size << "\t" << time_total << "\t" << time_broadcastdv << "\t" << time_getpath << "\t"
            << ttime_getpart << time_resizebuf << "\t" << time_write << "\t" << time_sync
            << "\n";

        log.close();
    }
#endif
}

ImgStore::ImgStore()
    : _log_base_len(0), _irequest(0), _started(false), _max_block_size(0)
{
    register_spmd(&ImgStore::bsp_store_img);
    register_spmd(&ImgStore::bsp_load_img);
    register_spmd(&ImgStore::bsp_load_img_seq);

    _dst.resize(PATH_MAX);
    _proc_ints_buffer.resize(bsp_nprocs());
    bsp_push_reg(&_dst[0], _dst.size());
    bsp_push_reg(&_nprocs_same_disk, sizeof(int));
    bsp_push_reg(&_proc_ints_buffer[0], bsp_nprocs() * sizeof(int));
    bsp_push_reg(_logpath, PATH_MAX);
    bsp_push_reg(&_log_base_len, sizeof(int));
    bsp_sync();
}

int ImgStore::storeImage(vector_distribution<char> img_data, std::string dst_base)
{
//    std::cout << "Store Image base path : " << dst_base << std::endl;
    _dv = img_data;
    memcpy(&_dst[0], &dst_base[0], dst_base.size() + 1);
    bsp_run(&ImgStore::bsp_store_img);

    return true;
}

void ImgStore::bsp_load_img()
{
#if LOG_MEASURES
    std::ofstream log;
    if (_log_base_len != 0)
    {
        sprintf(_logpath + _log_base_len, "_bsp_load_img_p%d", bsp_pid());

        log.open(_logpath, std::ofstream::out | std::ofstream::app);
        if (!log.is_open())
        {
            std::cout << "Could not open log for writing : " << _logpath << std::endl;
        }

        if (log.tellp() == 0)
        {
            log << "#block_size\ttime_total\ttime_getpath\ttime_computepartpath\ttime_readsize\ttime_alltoallblocksize"
                   "\ttime_maxblocksize\ttime_resizebuf\ttime_read\ttime_register\tirequest\n";
        }
    }

    struct timespec t0,t1,t2,t3,t4,t5,t6,t7,t8;

    t0 = now();
#endif

    char * base = &_dst[0];

    if (bsp_pid() == 0)
    {
        size_t base_len = strlen(base);
        for (int i = 1; i < bsp_nprocs(); ++i)
        {
            bsp_put(i, base, base, 0, base_len + 1);
        }
    }

    bsp_sync();

#if LOG_MEASURES
    t1 = now();
#endif

    char dir_path[PATH_MAX];
    int base_dir_len,base_len;

    base_len = strlen(base);
    base_dir_len = -1;

    for (int i = base_len; i--; )
    {
        if (base[i] == '/')
        {
            base_dir_len = i;
            break;
        }
    }

    if (base_dir_len == -1)
    {
        strcpy(dir_path, ".");
        base_dir_len = 1;
    }
    else
    {
        strncpy(dir_path, base, base_dir_len);
        dir_path[base_dir_len] = '\0';
    }

    sprintf(base + base_len, "_%d", bsp_pid());

    std::ifstream is(base);
    if (!is.is_open())
    {
        std::cout << "p" << bsp_pid() << " Failed to open file : " << base << std::endl;
        return;
    }

#if LOG_MEASURES
    t2 = now();
#endif

    is.seekg(0, std::ios_base::end);
    size_t block_size = is.tellg();
    is.seekg(0, std::ios_base::beg);

#if LOG_MEASURES
    t3 = now();
#endif

    for (int i = 0; i < bsp_nprocs(); ++i)
    {
        bsp_put(i, &block_size, &_proc_ints_buffer[0], bsp_pid() * sizeof(int), sizeof(int));
    }
    bsp_sync();

#if LOG_MEASURES
    t4 = now();
#endif

    _max_block_size = 0;
    for (size_t i = 0; i < _proc_ints_buffer.size(); ++i)
    {
        _max_block_size = std::max(_max_block_size, size_t(_proc_ints_buffer[i]));
    }

#if LOG_MEASURES
    t5 = now();
#endif

    _part.resize(_max_block_size);

#if LOG_MEASURES
    t6 = now();
#endif

    is.read(&_part[0], block_size);

#if LOG_MEASURES
    t7 = now();
#endif

    register_single_part_result(&_part[0], block_size, bsp_pid() * _max_block_size);
    bsp_sync();

#if LOG_MEASURES
    t8 = now();

    double time_total = diff_to_sec(&t0,&t8);
    double time_getpath = diff_to_sec(&t0,&t1);
    double time_computepartpath = diff_to_sec(&t1,&t2);
    double time_readsize  = diff_to_sec(&t2,&t3);
    double time_alltoallblocksize  = diff_to_sec(&t3,&t4);
    double time_maxblocksize  = diff_to_sec(&t4,&t5);
    double time_resizebuf  = diff_to_sec(&t5,&t6);
    double time_read  = diff_to_sec(&t6,&t7);
    double time_register  = diff_to_sec(&t7,&t8);
    double time_since_first = diff_to_sec(&_tfirst, &t0);

    if (log.is_open())
    {
        log << block_size << std::scientific
            << "\t" << time_total        << "\t" << time_getpath << "\t" << time_computepartpath
            << "\t" << time_readsize     << "\t" << time_alltoallblocksize
            << "\t" << time_maxblocksize << "\t" << time_resizebuf
            << "\t" << time_read         << "\t" << time_register
            << "\t" << _irequest++ << "\t" << time_since_first
            << "\n";

        log.close();
    }
#endif
}


vector_distribution<char> ImgStore::loadImage(std::string base_path)
{
#if LOG_MEASURES
    if (!_started)
    {
        _tfirst = now();
        _started = true;
    }
#endif

    strcpy(&_dst[0], base_path.c_str());

    bsp_run(&ImgStore::bsp_load_img);

    return gather_distr_parts<char>();
}

void ImgStore::bsp_load_img_seq()
{
    char * base = &_dst[0];

    if (bsp_pid() == 0)
    {
        size_t base_len = strlen(base);
        for (int i = 1; i < bsp_nprocs(); ++i)
        {
            bsp_put(i, base, base, 0, base_len + 1);
        }
    }

    bsp_sync();

    char dir_path[PATH_MAX];
    int base_dir_len,base_len;

    base_len = strlen(base);
    base_dir_len = -1;

    for (int i = base_len; i--; )
    {
        if (base[i] == '/')
        {
            base_dir_len = i;
            break;
        }
    }

    if (base_dir_len == -1)
    {
        strcpy(dir_path, ".");
        base_dir_len = 1;
    }
    else
    {
        strncpy(dir_path, base, base_dir_len);
        dir_path[base_dir_len] = '\0';
    }

    sprintf(base + base_len, "_%d", bsp_pid());

    std::ifstream is(base);
    if (!is.is_open())
    {
        std::cout << "p" << bsp_pid() << " Failed to open file : " << base << std::endl;
        return;
    }

    is.seekg(0, std::ios_base::end);
    size_t block_size = is.tellg();
    is.seekg(0, std::ios_base::beg);

    for (int s = 0; s < bsp_nprocs(); ++s)
    {
        bsp_put(s, &block_size, &_proc_ints_buffer[0], s * sizeof(int), sizeof(int));
    }
    bsp_sync();

    size_t prev_block_size = _max_block_size;

    _max_block_size = 0;
    for (size_t i = 0; i < _proc_ints_buffer.size(); ++i)
    {
        _max_block_size = std::max(_max_block_size, size_t(_proc_ints_buffer[i]));
    }

    _part.resize(_max_block_size);

    is.read(&_part[0], block_size);

    if (prev_block_size != _max_block_size)
    {
        if (prev_block_size != 0)
        {
            bsp_pop_reg(&_res[0]);
        }

        size_t res_size = bsp_pid() == 0 ? bsp_nprocs() * _max_block_size : 1;

        _res.resize(res_size);
        bsp_push_reg(&_res[0], res_size);

        bsp_sync();
    }

    bsp_put(0, &_part[0], &_res[0], bsp_pid() * _max_block_size, block_size);
    bsp_sync();
}

std::vector<char> ImgStore::loadImageSeq(std::string base_path)
{
    strcpy(&_dst[0], base_path.c_str());

    bsp_run(&ImgStore::bsp_load_img_seq);

    return _res;
}


int ImgStore::setLogPathBase(std::string path)
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

REGISTER_ACTOR(ImgStore)
