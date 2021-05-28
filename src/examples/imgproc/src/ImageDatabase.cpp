#include "ImageDatabase.h"

#include <string.h>
#include <stdio.h>

#include <fstream>
#include <algorithm>

#include <boost/functional/hash.hpp>

#include "div_range.h"
#include "measure.h"

REGISTER_ACTOR(ImageDatabase)

#define LOG_MEASURES 1

ImageDatabase::ImageDatabase()
    : _log_base_len(0), _irequest(0), _started(false)
{
    int s = bsp_pid();
    int p = bsp_nprocs();

    _procs_buf.resize(bsp_nprocs());

    _hash_block = HASH_SIZE / p;
    if (HASH_SIZE % p != 0)
    {
        ++_hash_block;
    }

    _hash_start = _hash_block * s;
    _nhash = (_hash_start + _hash_block <= HASH_SIZE ? _hash_block : HASH_SIZE - _hash_start);

    _hashTable.resize(_nhash);

    bsp_push_reg(&_procs_buf[0], _procs_buf.size() * sizeof(size_t));
    bsp_push_reg(&_width, sizeof(int));
    bsp_push_reg(&_height, sizeof(int));
    bsp_push_reg(&_nbitPix, sizeof(int));
    bsp_push_reg(_path, PATH_MAX);
    bsp_push_reg(&_global_match, sizeof(int));

    bsp_push_reg(_logpath, PATH_MAX);
    bsp_push_reg(&_log_base_len, sizeof(int));

    register_spmd(&ImageDatabase::bsp_insert);
    register_spmd(&ImageDatabase::bsp_insert_seq);
    register_spmd(&ImageDatabase::bsp_save_db);
    register_spmd(&ImageDatabase::bsp_load_db);
    register_spmd(&ImageDatabase::bsp_set_img_params);
    register_spmd(&ImageDatabase::bsp_exist);

    bsp_sync();
}

int ImageDatabase::setLogPathBase(std::string path)
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

void ImageDatabase::bsp_set_img_params()
{
    int s = bsp_pid();
    int p = bsp_nprocs();

    if (s != 0)
    {
        bsp_get(0, &_width, 0, &_width, sizeof(int));
        bsp_get(0, &_height, 0, &_height, sizeof(int));
        bsp_get(0, &_nbitPix, 0, &_nbitPix, sizeof(int));
    }

    bsp_sync();

    _full_size = _width * _height;

    //_block_size = _full_size / p;

    unsigned long part_end;
    div_range(_full_size, s, p, &_part_offset, &part_end);

    //_part_offset = _block_size * s;
    //_part_size = _part_offset + _block_size <= _full_size ? _block_size : _full_size - _part_offset;

    _part_size = part_end - _part_offset;

    if (_part.size() != 0)
    {
        bsp_pop_reg(&_part[0]);
    }
    _part.resize(_part_size);
    _thumbnail_buf.resize(_full_size);
    bsp_push_reg(&_part[0], _part_size * sizeof(char));
    bsp_push_reg(&_thumbnail_buf[0], _full_size * sizeof(char));
    bsp_sync();
}

int ImageDatabase::setImgParams(int width, int height, int nbitPix)
{
    _width = width;
    _height = height;
    _nbitPix = nbitPix;

    bsp_run(&ImageDatabase::bsp_set_img_params);

    return 0;
}

int ImageDatabase::insert(Future<vector_distribution<char> > img)
{
#if LOG_MEASURES
    if (!_started)
    {
        _tfirst = now();
        _started = true;
    }
#endif

    _dv = img.get();
    bsp_run(&ImageDatabase::bsp_insert);

    dv_release(_dv);
    img.release();

    return 0;
}

void ImageDatabase::bsp_insert()
{
#if LOG_MEASURES
    std::ofstream log;
    if (_log_base_len != 0)
    {
        sprintf(_logpath + _log_base_len, "_bsp_insert_p%d", bsp_pid());

        log.open(_logpath, std::ofstream::out | std::ofstream::app);
        if (!log.is_open())
        {
            std::cout << "Could not open log for writing : " << _logpath << std::endl;
        }

        if (log.tellp() == 0)
        {
            log << "#part_size\ttime_total"
                   "\ttime_bcdv\ttime_getpart\ttime_localhash\ttime_bchash"
                   "\ttime_globhash\ttime_gatherparts\t"
                   "time_fndhashentry\ttime_insertthumb"
                   "\ttime_chkcollisions\ttime_insertthumbref\ttime\n";
        }
    }

    struct timespec t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10;

    t0 = now();
#endif

    broadcast_dv(_dv);

#if LOG_MEASURES
    t1 = now();
#endif

    dv_get_part(_dv, _part_offset, &_part[0], _part_size);

#if LOG_MEASURES
    t2 = now();
#endif

    size_t local_hash = 0;
    boost::hash_range(local_hash, _part.begin(), _part.end());

    std::vector<size_t> & procs_hash = _procs_buf;

#if LOG_MEASURES
    t3 = now();
#endif

    for (int i = 0; i < bsp_nprocs(); ++i)
    {
        bsp_put(i, &local_hash, &procs_hash[0], bsp_pid() * sizeof(size_t), sizeof(size_t));
    }

    bsp_sync();

#if LOG_MEASURES
    t4 = now();
#endif

    size_t global_hash = 0;
    boost::hash_range(global_hash, procs_hash.begin(), procs_hash.end());

#if LOG_MEASURES
    t5 = now();
#endif

    size_t modulus_hash = global_hash % HASH_SIZE;

    int dst_proc = modulus_hash / _hash_block;

    bsp_put(dst_proc, &_part[0], &_thumbnail_buf[0], _part_offset, _part_size);
    bsp_sync();

#if LOG_MEASURES
    t6 = t7 = t8 = t9 = t10 = now();
#endif

    if (bsp_pid() == dst_proc)
    {
        size_t hash_index = modulus_hash - _hash_start;
        std::list<std::vector<char>*> & hash_entry = _hashTable[hash_index];

#if LOG_MEASURES
        t7 = now();
#endif

        _thumbnails.push_back(_thumbnail_buf);

#if LOG_MEASURES
        t8 = now();
#endif

        bool found = false;

        //check collisions

        for (auto it = hash_entry.begin(); it != hash_entry.end(); ++it)
        {
            if (std::equal((*it)->begin(), (*it)->end(), _thumbnail_buf.begin()))
            {
                found = true;
                break;
            }
        }

#if LOG_MEASURES
        t9 = now();
#endif

        if (!found)
        {
            hash_entry.push_back(&_thumbnails.back());
        }

#if LOG_MEASURES
        t10 = now();
#endif

    }

    bsp_sync();

#if LOG_MEASURES
    double time_total = diff_to_sec(&t0,&t10);
    double time_bcdv = diff_to_sec(&t0,&t1);
    double time_getpart = diff_to_sec(&t1,&t2);
    double time_localhash  = diff_to_sec(&t2,&t3);
    double time_bchash  = diff_to_sec(&t3,&t4);
    double time_globhash  = diff_to_sec(&t4,&t5);
    double time_gatherpart  = diff_to_sec(&t5,&t6);
    double time_fndhashentry  = diff_to_sec(&t6,&t7);
    double time_insertthumb  = diff_to_sec(&t7,&t8);
    double time_chkcollisions  = diff_to_sec(&t8,&t9);
    double time_insertthumbref  = diff_to_sec(&t9,&t10);
    double time_since_first = diff_to_sec(&_tfirst, &t0);

    if (log.is_open())
    {
        log << _part_size << std::scientific
            << "\t" << time_total
            << "\t" << time_bcdv        << "\t" << time_getpart
            << "\t" << time_localhash     << "\t" << time_bchash
            << "\t" << time_globhash << "\t" << time_gatherpart
            << "\t" << time_fndhashentry << "\t" << time_insertthumb
            << "\t" << time_chkcollisions << "\t" << time_insertthumbref
            << "\t" << _irequest++ << "\t" << time_since_first
            << "\n";

        log.close();
    }
#endif
}

int  ImageDatabase::insertSeq(Future<std::vector<char> > img)
{
    _img = img.get();

    for (int i = 0; i < bsp_nprocs(); ++i)
    {
        unsigned long part_end, part_offset, part_size;
        div_range(_full_size, i, bsp_nprocs(), &part_offset, &part_end);
        part_size = part_end - part_offset;

        _part_size = part_end - _part_offset;

        bsp_put(i, &_img[0] + part_offset, &_part[0], 0, part_size);
    }

    bsp_run(&ImageDatabase::bsp_insert_seq);

    return 0;
}

void ImageDatabase::bsp_insert_seq()
{

    bsp_sync();

    size_t local_hash = 0;
    boost::hash_range(local_hash, _part.begin(), _part.end());

    std::vector<size_t> & procs_hash = _procs_buf;

    for (int i = 0; i < bsp_nprocs(); ++i)
    {
        bsp_put(i, &local_hash, &procs_hash[0], bsp_pid() * sizeof(size_t), sizeof(size_t));
    }

    bsp_sync();

    size_t global_hash = 0;
    boost::hash_range(global_hash, procs_hash.begin(), procs_hash.end());

    size_t modulus_hash = global_hash % HASH_SIZE;

    int dst_proc = modulus_hash / _hash_block;

    bsp_put(dst_proc, &_part[0], &_thumbnail_buf[0], _part_offset, _part_size);
    bsp_sync();

    if (bsp_pid() == dst_proc)
    {
        size_t hash_index = modulus_hash - _hash_start;
        std::list<std::vector<char>*> & hash_entry = _hashTable[hash_index];

        _thumbnails.push_back(_thumbnail_buf);

        bool found = false;

        //check collisions

        for (auto it = hash_entry.begin(); it != hash_entry.end(); ++it)
        {
            if (std::equal((*it)->begin(), (*it)->end(), _thumbnail_buf.begin()))
            {
                found = true;
                break;
            }
        }

        if (!found)
        {
            hash_entry.push_back(&_thumbnails.back());
        }

    }

    bsp_sync();

}

void ImageDatabase::bsp_load_db()
{
    if (bsp_pid() != 0)
    {
        bsp_get(0, _path, 0, _path, PATH_MAX);
    }

    bsp_sync();

    int base_len = strlen(_path);
    base_len += snprintf(_path + base_len, PATH_MAX - base_len, "imgdb_");

    for (size_t i = 0; ; ++i)
    {
        snprintf(_path + base_len, PATH_MAX - base_len, "file%zu_p%d", i, bsp_pid());

        std::ifstream is(_path);

        if (!is.is_open())
        {
            break;
        }

        size_t hash;
        is.read((char *) &hash, sizeof(size_t));
        is.read(&_thumbnail_buf[0], _full_size);

        hash -= _hash_start;

        _thumbnails.push_back(_thumbnail_buf);
        _hashTable[hash].push_back(&_thumbnails.back());

        is.close();

//        std::cout << "p" << bsp_pid() << " loaded thumbnail with hash value : " << hash <<  std::endl;
    }
}

int ImageDatabase::loadDb(std::string dir)
{
    if (dir[dir.size() - 1] != '/')
    {
        dir += "/";
    }

    strncpy(_path, dir.c_str(), PATH_MAX);

    bsp_run(&ImageDatabase::bsp_load_db);

    return 0;
}

void ImageDatabase::bsp_save_db()
{
    bsp_sync();

    int base_len = strlen(_path);
    base_len += snprintf(_path + base_len, PATH_MAX - base_len, "imgdb_");

    size_t hash = _hash_start;
    size_t i = 0;
    for (auto hash_it = _hashTable.begin(); hash_it != _hashTable.end(); ++hash_it, ++hash)
    {
        for (auto thumb_it = hash_it->begin(); thumb_it != hash_it->end(); ++thumb_it, ++i)
        {
            snprintf(_path + base_len, PATH_MAX - base_len, "file%zu_p%d", i, bsp_pid());
            std::ofstream os(_path);

            os.write((char *) &hash, sizeof(size_t));
            os.write(&(*thumb_it)->front(), (*thumb_it)->size());
            os.close();
        }
    }

    bsp_sync();
}

int ImageDatabase::saveDb(std::string dir)
{
    if (dir[dir.size()] != '/')
    {
        dir += "/";
    }

    strncpy(_path, dir.c_str(), PATH_MAX);
    size_t path_len = strlen(_path);
    for (int s = 1; s < bsp_nprocs(); ++s)
    {
        bsp_put(s, _path, _path, 0, path_len + 1);
    }

    bsp_run(&ImageDatabase::bsp_save_db);

    return 0;
}

void ImageDatabase::bsp_exist()
{
    broadcast_dv(_dv);

    dv_get_part(_dv, _part_offset, &_part[0], _part_size);

    size_t local_hash = 0;
    boost::hash_range(local_hash, _part.begin(), _part.end());

//    std::cout << "p" << bsp_pid() << " : local hash : " << local_hash << std::endl;

    std::vector<size_t> & procs_hash = _procs_buf;

    for (int i = 0; i < bsp_nprocs(); ++i)
    {
        bsp_put(i, &local_hash, &procs_hash[0], bsp_pid() * sizeof(size_t), sizeof(size_t));
    }

    bsp_sync();

    size_t global_hash = 0;
    boost::hash_range(global_hash, procs_hash.begin(), procs_hash.end());

//    std::cout << "p" << bsp_pid() << " : global hash : " << global_hash << std::endl;

    size_t modulus_hash = global_hash % HASH_SIZE;
//    std::cout << "p" << bsp_pid() << " : modulus_hash hash : " << modulus_hash << std::endl;

    int dst_proc = modulus_hash / _hash_block;

    bsp_put(dst_proc, &_part[0], &_thumbnail_buf[0], _part_offset, _part_size);
    bsp_sync();

    if (bsp_pid() == dst_proc)
    {
//        std::cout << "p" << bsp_pid() << " chosen for checking its db" << std::endl;
//        std::cout << "p" << bsp_pid() << " has " << _thumbnails.size() << " thumbails stored" << std::endl;

        size_t hash_index = modulus_hash - _hash_start;
        std::list<std::vector<char>*> & hash_entry = _hashTable[hash_index];

//        std::cout << "p" << bsp_pid() << " hash entry has " << hash_entry.size() << " corresponding element" << std::endl;

        int found = 0;

        //check collisions

        for (auto it = hash_entry.begin(); it != hash_entry.end(); ++it)
        {
            if (std::equal((*it)->begin(), (*it)->end(), _thumbnail_buf.begin()))
            {
                found = 1;
                break;
            }
        }

        for (int i = 0; i < bsp_nprocs(); ++i)
        {
            bsp_put(0, &found, &_global_match, 0, sizeof(int));
        }
    }

    bsp_sync();
}

int ImageDatabase::exist(Future<vector_distribution<char> > img)
{
    _dv = img.get();

    bsp_run(&ImageDatabase::bsp_exist);

    return _global_match;
}
