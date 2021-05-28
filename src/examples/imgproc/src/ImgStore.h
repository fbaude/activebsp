#ifndef __IMGSTORE_H__
#define __IMGSTORE_H__

#include "Actor.h"
#include "decl_actor.h"
#include "vector_distribution.h"

using namespace activebsp;

class ImgStore : public activebsp::ActorBase
{
private:
    char _logpath[PATH_MAX];
    size_t _log_base_len;
    int _irequest;
    struct timespec _tfirst;
    bool _started;

    vector_distribution<char> _dv;
    std::vector<char> _part;
    std::vector<char> _res;
    std::vector<char> _dst;

    size_t _disk_block_index;
    size_t _disk_nblocks;
    size_t _max_block_size;
    int _nprocs_same_disk;

    std::vector<int> _proc_ints_buffer;

    void bsp_store_img();
    void bsp_load_img();
    void bsp_load_img_seq();

public:
    ImgStore();

    int storeImage(vector_distribution<char> img_data, std::string dst_base);
    vector_distribution<char> loadImage(std::string base_path);
    std::vector<char> loadImageSeq(std::string base_path);

    int setLogPathBase(std::string path);
};

DECL_ACTOR(ImgStore,
          (int, storeImage, vector_distribution<char>, std::string),
          (vector_distribution<char>, loadImage, std::string),
          (std::vector<char>, loadImageSeq, std::string),
          (int, setLogPathBase, std::string)
)

#endif // __IMGSTORE_H__
