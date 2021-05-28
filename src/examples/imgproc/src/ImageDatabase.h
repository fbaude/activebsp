#ifndef __IMAGE_DATABASE_H__
#define __IMAGE_DATABASE_H__

#include <list>

#include "Actor.h"
#include "decl_actor.h"

#define HASH_SIZE (12289)

using namespace activebsp;

class ImageDatabase : public ActorBase
{
private:
    int _width;
    int _height;
    int _nbitPix;

    size_t _block_size;
    size_t _part_offset;
    size_t _part_size;
    size_t _full_size;

    std::vector<size_t> _procs_buf;
    int _global_match;

    char _path[PATH_MAX];

    std::list<std::vector<char> > _partsDb;

    std::list<std::vector<char> >_thumbnails;
    std::vector<std::list<std::vector<char>*> > _hashTable;
    size_t _hash_start;
    size_t _nhash;
    size_t _hash_block;

    std::vector<char> _thumbnail_buf;

    vector_distribution<char> _dv;
    std::vector<char> _part;

    std::vector<char> _img;

    char _logpath[PATH_MAX];
    size_t _log_base_len;
    int _irequest;
    struct timespec _tfirst;
    bool _started;

    void bsp_set_img_params();
    void bsp_insert();
    void bsp_insert_seq();
    void bsp_save_db();
    void bsp_load_db();
    void bsp_exist();

public:
    ImageDatabase();

    int setImgParams(int width, int height, int nbitPix);

    int insert(Future<vector_distribution<char> > img);
    int insertSeq(Future<std::vector<char> > img);

    int loadDb(std::string dir);
    int saveDb(std::string dir);

    int exist(Future<vector_distribution<char> > img);

    int setLogPathBase(std::string path);

};

DECL_ACTOR(ImageDatabase,
          (int, insert, Future<vector_distribution<char> >),
          (int, insertSeq, Future<std::vector<char> >),
          (int, setImgParams, int, int, int),
          (int, saveDb, std::string),
//          (int, loadDb, std::string),
          (int, setLogPathBase, std::string)
          //(int, exist, Future<vector_distribution<char> >)
)

#endif // __IMAGE_DATABASE_H__
