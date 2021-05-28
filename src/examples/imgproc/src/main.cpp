#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <jpeglib.h>

#include "activebsp.h"
#include "Future.h"
#include "Proxy.h"
#include "vector_distribution.h"

#include "ImgStore.h"
#include "MonochromeCompressor.h"
#include "ImageDatabase.h"
#include "measure.h"

using namespace std;
using namespace activebsp;

#define COMPRESS_WIDTH 64
#define COMPRESS_HEIGHT 64
#define COMPRESS_BIT_PER_PIXEL 8

#define MEASURE_SEQUENCES 0

void help(const char * pname)
{
    printf("Usage : %s disk_procs compressor_procs db_procs cmd cmdargs...\n", pname);
}

void help_store()
{
    printf("Store cmd args : src_local dst_base\n");
}

void storeImageOnDisk(Proxy<ImgStore> & disk, const char * src_local, const char * dst_base)
{
    int rc;
    size_t i;
    struct stat file_info;
    unsigned long jpg_size;
    unsigned char *jpg_buffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // Variables for the output buffer, and how long each row is
    unsigned long bmp_size;
    unsigned char *bmp_buffer;
    int row_stride, width, height, pixel_size;

    rc = stat(src_local, &file_info);
    if (rc) {
        printf("FAILED to stat source jpg");
        return;
    }

    jpg_size = file_info.st_size;
    jpg_buffer = (unsigned char*) malloc(jpg_size + 100);


    int fd = open(src_local, O_RDONLY);
    i = 0;
    while (i < jpg_size) {
        rc = read(fd, jpg_buffer + i, jpg_size - i);
        syslog(LOG_INFO, "Input: Read %d/%lu bytes", rc, jpg_size-i);
        i += rc;
    }
    close(fd);


    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);
    rc = jpeg_read_header(&cinfo, TRUE);

    if (rc != 1) {
        printf("File does not seem to be a normal JPEG");
        return;
    }

    jpeg_start_decompress(&cinfo);

    width = cinfo.output_width;
    height = cinfo.output_height;
    pixel_size = cinfo.output_components;

    char buf[1024];
    rc = sprintf(buf, "P6 %d %d 255\n", width, height);
    bmp_size = rc + width * height * pixel_size;
    bmp_buffer = (unsigned char*) malloc(bmp_size);
    memcpy(bmp_buffer, buf, rc);
    unsigned char * bmp_data_start = bmp_buffer + rc;

    row_stride = width * pixel_size;

    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *buffer_array[1];
        buffer_array[0] = bmp_data_start + \
                           (cinfo.output_scanline) * row_stride;

        jpeg_read_scanlines(&cinfo, buffer_array, 1);

    }

    jpeg_finish_decompress(&cinfo);

    jpeg_destroy_decompress(&cinfo);

    vector_distribution<char> dv = disk.block_distribute((char *) bmp_buffer, bmp_size);
    disk.storeImage(dv, dst_base).get();

    dv_release(dv);

    free(bmp_buffer);
    free(jpg_buffer);
}

void help_load()
{
    printf("load cmd args : src_base dst\n");
}

void loadImg(Proxy<ImgStore> & disk, const char * base, const char * dst)
{
    Future<vector_distribution<char> > fdv = disk.loadImage(base);
    vector_distribution<char> dv = fdv.get();

    size_t size = dv.getVecSize();
    std::vector<char> img(size);
    dv_get_part(dv, 0, &img[0], size);

    std::ofstream os(dst);
    os.write(&img[0], size);
    os.close();

    fdv.release();
    dv_release(dv);
}

void help_compress()
{
    printf("compress cmd args : src_base dst width height\n");
}

void compressImg(Proxy<ImgStore> & disk, Proxy<MonochromeCompressor> & compressor,
                 const char * base, const char * dst, int width, int height)
{
    Future<vector_distribution<char> > img = disk.loadImage(base);
    Future<vector_distribution<char> > compressed = compressor.compress(img, width, height, false, COMPRESS_BIT_PER_PIXEL);

    vector_distribution<char> dv = compressed.get();

    size_t size = dv.getVecSize();
    std::vector<char> data(size);
    dv_get_part(dv, 0, &data[0], size);

    std::ofstream os(dst);
    os.write(&data[0], size);
    os.close();

    for (int i = 0; i < dv.nparts(); ++i)
    {
        vector_distribution_part_t * part =  dv.getPart(i);

        std::cout << "part offset " << part->offset << ", size " << part->size << std::endl;
    }

    /*
    dv_release(img.get());
    img.release();
    dv_release(compressed.get());
    compressed.release();
    */
}

void help_fillDb()
{
    printf("fill_db cmd args : src_dir db_path\n");
}

void fillDb(Proxy<ImgStore> & disk, Proxy<MonochromeCompressor> & compressor, Proxy<ImageDatabase> & db,
            const char * imgDir, const char * dbSaveDir)
{
    UNUSED(disk);
    db.setImgParams(COMPRESS_WIDTH, COMPRESS_HEIGHT, COMPRESS_BIT_PER_PIXEL).get();

    int rc;
    size_t i;
    struct stat file_info;
    unsigned long jpg_size, jpg_buf_size;
    unsigned char *jpg_buffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // Variables for the output buffer, and how long each row is
    unsigned long bmp_size, bmp_buf_size;
    unsigned char *bmp_buffer;
    int row_stride, width, height, pixel_size;

    char path[PATH_MAX];
    size_t img_dir_len = strlen(imgDir);

    strcpy(path, imgDir);
    if (path[img_dir_len - 1] != '/')
    {
        path[img_dir_len++] = '/';
        path[img_dir_len]   = '\0';
    }

    jpg_buf_size = 0;
    bmp_buf_size = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (imgDir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            size_t ent_size = strlen(ent->d_name);

            if ( ent_size > 5 && (strncmp(ent->d_name + ent_size - 4, ".jpg", 4) == 0))
            {
                strncpy(path + img_dir_len, ent->d_name, PATH_MAX - img_dir_len);

                rc = stat(path, &file_info);
                if (rc) {
                    printf("FAILED to stat source jpg");
                    return;
                }

                jpg_size = file_info.st_size;

                if (jpg_buf_size < jpg_size)
                {
                    if (jpg_buf_size != 0)
                    {
                        free(jpg_buffer);
                    }

                    jpg_buf_size = jpg_size;

                    jpg_buffer = (unsigned char*) malloc(jpg_buf_size + 100);
                }


                int fd = open(path, O_RDONLY);
                i = 0;
                while (i < jpg_size) {
                    rc = read(fd, jpg_buffer + i, jpg_size - i);
                    syslog(LOG_INFO, "Input: Read %d/%lu bytes", rc, jpg_size-i);
                    i += rc;
                }
                close(fd);

                jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);
                rc = jpeg_read_header(&cinfo, TRUE);

                if (rc != 1) {
                    printf("File does not seem to be a normal JPEG");
                    return;
                }

                jpeg_start_decompress(&cinfo);

                width = cinfo.output_width;
                height = cinfo.output_height;
                pixel_size = cinfo.output_components;

                char buf[1024];
                rc = sprintf(buf, "P6 %d %d 255\n", width, height);

                bmp_size = rc + width * height * pixel_size;

                if (bmp_buf_size < bmp_size)
                {
                    if (bmp_buf_size != 0)
                    {
                        free(bmp_buffer);
                    }

                    bmp_buf_size = bmp_size;

                    bmp_buffer = (unsigned char*) malloc(bmp_size);
                }

                memcpy(bmp_buffer, buf, rc);
                unsigned char * bmp_data_start = bmp_buffer + rc;

                row_stride = width * pixel_size;

                while (cinfo.output_scanline < cinfo.output_height) {
                    unsigned char *buffer_array[1];
                    buffer_array[0] = bmp_data_start + \
                                       (cinfo.output_scanline) * row_stride;

                    jpeg_read_scanlines(&cinfo, buffer_array, 1);

                }

                jpeg_finish_decompress(&cinfo);

                vector_distribution<char> img = compressor.block_distribute((char *) bmp_buffer, bmp_size);
                Future<vector_distribution<char> > compressed = compressor.compressDv(img, COMPRESS_WIDTH, COMPRESS_HEIGHT, true, COMPRESS_BIT_PER_PIXEL);

                vector_distribution<char> dv = compressed.get();
                size_t vec_size = dv.getVecSize();
                std::cout << "Vecsize is : " << vec_size << std::endl;
                std::vector<char> res = dv_get_part(dv, 0, vec_size);
                db.insert(compressed).get();
            }
      }
      closedir (dir);
    } else
    {
        std::cout << "Could not open base directory" << std::endl;
    }


    db.saveDb(dbSaveDir).get();

    jpeg_destroy_decompress(&cinfo);
}

void help_search_img()
{
    printf("search_img cmd args : img_path db_path\n");
}

void searchImg(Proxy<ImgStore> & disk, Proxy<MonochromeCompressor> & compressor, Proxy<ImageDatabase> & db,
               const char * img_path, const char * db_path)
{
    UNUSED(disk);
    UNUSED(compressor);
    UNUSED(db);
    UNUSED(img_path);
    UNUSED(db_path);

    // may work but not required and not checked in a long time so disabled until checked

    std::cout << "searchImg DISABLED" << std::endl;
    return;
/*
    db.setImgParams(COMPRESS_WIDTH, COMPRESS_HEIGHT, COMPRESS_BIT_PER_PIXEL).get();
    db.loadDb(db_path).get();

    Future<vector_distribution> img = disk.loadImage(img_path);
    Future<vector_distribution> compressed = compressor.compress(img, COMPRESS_WIDTH, COMPRESS_HEIGHT, true, COMPRESS_BIT_PER_PIXEL);


    std::cout << "searchImg DISABLED" << std::endl;
    return;
*/
    /*
    if (db.exist(compressed).get() == 1)
    {
        std::cout << "Image found in DB" << std::endl;
    }
    else
    {
        std::cout << "Image NOT found in DB" << std::endl;
    }
    */

}

void help_store_dir()
{
    printf("store_dir cmd args : img_dir disk_dir\n");
}


void help_store_ppm_dir()
{
    printf("store_ppm_dir cmd args : img_dir disk_dir\n");
}

void storePPMDirOnDisk(Proxy<ImgStore> & disk, const char * imgDir, const char * diskDir)
{
    unsigned long bmp_size, bmp_buf_size;
    unsigned char *bmp_buffer;

    char src_path[PATH_MAX];
    size_t img_dir_len = strlen(imgDir);

    strcpy(src_path, imgDir);
    if (src_path[img_dir_len - 1] != '/')
    {
        src_path[img_dir_len++] = '/';
        src_path[img_dir_len]   = '\0';
    }

    char dst_path[PATH_MAX];
    size_t disk_dir_len = strlen(diskDir);

    strcpy(dst_path, diskDir);
    if (dst_path[disk_dir_len - 1] != '/')
    {
        dst_path[disk_dir_len++] = '/';
        dst_path[disk_dir_len]   = '\0';
    }

    bmp_buf_size = 0;

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (imgDir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            size_t ent_size = strlen(ent->d_name);

            if ( ent_size > 5 && (strncmp(ent->d_name + ent_size - 4, ".ppm", 4) == 0))
            {
                strncpy(src_path + img_dir_len, ent->d_name, PATH_MAX - img_dir_len);

                std::ifstream is(src_path);
                is.seekg(0, std::ios_base::end);
                bmp_size = is.tellg();
                is.seekg(0, std::ios_base::beg);

                if (bmp_buf_size < bmp_size)
                {
                    if (bmp_buf_size != 0)
                    {
                        free(bmp_buffer);
                    }

                    bmp_buf_size = bmp_size;

                    bmp_buffer = (unsigned char*) malloc(bmp_size);
                }

                is.read((char*) bmp_buffer, bmp_size);
                is.close();

                snprintf(dst_path + disk_dir_len, PATH_MAX - disk_dir_len, "%s", ent->d_name);

                vector_distribution<char> img = disk.block_distribute((char *) bmp_buffer, bmp_size);

                disk.storeImage(img, dst_path).get();
            }
      }
      closedir (dir);
    } else
    {
        std::cout << "Could not open base directory" << std::endl;
    }
}

void storeImagesOnDisk(Proxy<ImgStore> & disk, const char * imgDir, const char * diskDir)
{
    int rc;
    size_t i;
    struct stat file_info;
    unsigned long jpg_size, jpg_buf_size;
    unsigned char *jpg_buffer;

    // Variables for the decompressor itself
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    // Variables for the output buffer, and how long each row is
    unsigned long bmp_size, bmp_buf_size;
    unsigned char *bmp_buffer;
    int row_stride, width, height, pixel_size;

    char src_path[PATH_MAX];
    size_t img_dir_len = strlen(imgDir);

    strcpy(src_path, imgDir);
    if (src_path[img_dir_len - 1] != '/')
    {
        src_path[img_dir_len++] = '/';
        src_path[img_dir_len]   = '\0';
    }

    char dst_path[PATH_MAX];
    size_t disk_dir_len = strlen(diskDir);

    strcpy(dst_path, diskDir);
    if (dst_path[disk_dir_len - 1] != '/')
    {
        dst_path[disk_dir_len++] = '/';
        dst_path[disk_dir_len]   = '\0';
    }

    jpg_buf_size = 0;
    bmp_buf_size = 0;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir (imgDir)) != NULL)
    {
        while ((ent = readdir (dir)) != NULL)
        {
            size_t ent_size = strlen(ent->d_name);

            if ( ent_size > 5 && (strncmp(ent->d_name + ent_size - 4, ".jpg", 4) == 0))
            {
                strncpy(src_path + img_dir_len, ent->d_name, PATH_MAX - img_dir_len);

                rc = stat(src_path, &file_info);
                if (rc) {
                    printf("FAILED to stat source jpg");
                    return;
                }

                jpg_size = file_info.st_size;

                if (jpg_buf_size < jpg_size)
                {
                    if (jpg_buf_size != 0)
                    {
                        free(jpg_buffer);
                    }

                    jpg_buf_size = jpg_size;

                    jpg_buffer = (unsigned char*) malloc(jpg_buf_size + 100);
                }

                int fd = open(src_path, O_RDONLY);
                i = 0;
                while (i < jpg_size) {
                    rc = read(fd, jpg_buffer + i, jpg_size - i);
                    syslog(LOG_INFO, "Input: Read %d/%lu bytes", rc, jpg_size-i);
                    i += rc;
                }
                close(fd);

                jpeg_mem_src(&cinfo, jpg_buffer, jpg_size);
                rc = jpeg_read_header(&cinfo, TRUE);

                if (rc != 1) {
                    printf("File does not seem to be a normal JPEG");
                    return;
                }

                jpeg_start_decompress(&cinfo);

                width = cinfo.output_width;
                height = cinfo.output_height;
                pixel_size = cinfo.output_components;

                char buf[1024];
                rc = sprintf(buf, "P6 %d %d 255\n", width, height);

                bmp_size = rc + width * height * pixel_size;

                if (bmp_buf_size < bmp_size)
                {
                    if (bmp_buf_size != 0)
                    {
                        free(bmp_buffer);
                    }

                    bmp_buf_size = bmp_size;

                    bmp_buffer = (unsigned char*) malloc(bmp_size);
                }

                memcpy(bmp_buffer, buf, rc);
                unsigned char * bmp_data_start = bmp_buffer + rc;

                row_stride = width * pixel_size;

                while (cinfo.output_scanline < cinfo.output_height) {
                    unsigned char *buffer_array[1];
                    buffer_array[0] = bmp_data_start + \
                                       (cinfo.output_scanline) * row_stride;

                    jpeg_read_scanlines(&cinfo, buffer_array, 1);
                }

                jpeg_finish_decompress(&cinfo);

                snprintf(dst_path + disk_dir_len, PATH_MAX - disk_dir_len, "%s", ent->d_name);

                vector_distribution<char> img = disk.block_distribute((char *) bmp_buffer, bmp_size);

                std::cout << "storing on disk with base path : " << dst_path << std::endl;
                disk.storeImage(img, dst_path).get();
            }
      }
      closedir (dir);
    } else
    {
        std::cout << "Could not open base directory" << std::endl;
    }

    jpeg_destroy_decompress(&cinfo);
}

void help_insert_dir()
{
    printf("insert_list cmd args : base_list_file db_dir\n");
}

void insertDir(Proxy<ImgStore> & disk, Proxy<MonochromeCompressor> & compressor, Proxy<ImageDatabase> & db,
               const char * base_list_file, const char * db_dir, const char * measure_path)
{
    std::string path;

    std::ifstream is(base_list_file);
    if (!is.is_open())
    {
        std::cout << "Could not open base_list_file : " << base_list_file << std::endl;
        return;
    }

    if (measure_path != NULL)
    {
        disk.setLogPathBase(measure_path).release();
        compressor.setLogPathBase(measure_path).release();
        db.setLogPathBase(measure_path).release();
    }

    db.setImgParams(COMPRESS_WIDTH, COMPRESS_HEIGHT, COMPRESS_BIT_PER_PIXEL).release();

    int nimg = 0;
    struct timespec t0,t1,t2;

#if MEASURE_SEQUENCES
    struct timespec tbeg, tload,tcompress,tinsert;
    double time_load, time_compress, time_insert;
    time_load = time_compress = time_insert = 0.0;
#endif

    t0 = now();

    while (getline(is, path)) {
#if MEASURE_SEQUENCES
        tbeg = now();
        Future<vector_distribution<char> > img = disk.loadImage(path);
        img.get();
        tload = now();
        Future<vector_distribution<char> > compressed = compressor.compress(img, COMPRESS_WIDTH, COMPRESS_HEIGHT, true, COMPRESS_BIT_PER_PIXEL);
        compressed.get();
        tcompress = now();
        db.insert(compressed).get();
        tinsert = now();

        time_load += diff_to_sec(&tbeg,&tload);
        time_compress += diff_to_sec(&tload,&tcompress);
        time_insert += diff_to_sec(&tcompress,&tinsert);
#else
        Future<vector_distribution<char> > img = disk.loadImage(path);
        Future<vector_distribution<char> > compressed = compressor.compress(img, COMPRESS_WIDTH, COMPRESS_HEIGHT, true, COMPRESS_BIT_PER_PIXEL);
        db.insert(compressed).release();
#endif
        ++nimg;
    }

    if (is.bad()) {
        std::cout << "IO error" << std::endl;
    }

    t1 = now();

    db.saveDb(db_dir).get();

    t2 = now();

    double time_calls = diff_to_sec(&t0,&t1);
    double time_process = diff_to_sec(&t0,&t2);

    if (measure_path != NULL)
    {
        std::string logpath(measure_path);
        logpath += "_main";
        std::ofstream os(logpath, std::ofstream::out | std::ofstream::app);
        if (!os.is_open())
        {
            std::cout << "Could not write measure to path " << measure_path << std::endl;
        }
        else
        {
#if MEASURE_SEQUENCES
            time_load /= double(nimg);
            time_compress /= double(nimg);
            time_insert /= double(nimg);

            os << nimg << std::scientific << "\t" << time_calls << "\t" << time_process << "\t" << time_load << "\t" << time_compress << "\t" << time_insert << "\n";
#else

            os << nimg << std::scientific << "\t" << time_calls << "\t" << time_process << "\n";
#endif
            os.close();
        }

    }

}

void insertDirSeq(Proxy<ImgStore> & disk, Proxy<MonochromeCompressor> & compressor, Proxy<ImageDatabase> & db,
               const char * base_list_file, const char * db_dir, const char * measure_path)
{
    std::string path;

    std::ifstream is(base_list_file);
    if (!is.is_open())
    {
        std::cout << "Could not open base_list_file : " << base_list_file << std::endl;
        return;
    }

    if (measure_path != NULL)
    {
        disk.setLogPathBase(measure_path).release();
        compressor.setLogPathBase(measure_path).release();
        db.setLogPathBase(measure_path).release();
    }

    db.setImgParams(COMPRESS_WIDTH, COMPRESS_HEIGHT, COMPRESS_BIT_PER_PIXEL).release();

    int nimg = 0;
    struct timespec t0,t1,t2;
#if MEASURE_SEQUENCES
    struct timespec tbeg, tload,tcompress,tinsert;
    double time_load, time_compress, time_insert;
    time_load = time_compress = time_insert = 0.0;
#endif

    t0 = now();

    while (getline(is, path)) {
#if MEASURE_SEQUENCES
        tbeg = now();
        Future<std::vector<char> > img = disk.loadImageSeq(path);
        img.get();
        tload = now();
        Future<std::vector<char> > compressed = compressor.compressSeq(img, COMPRESS_WIDTH, COMPRESS_HEIGHT, true, COMPRESS_BIT_PER_PIXEL);
        compressed.get();
        tcompress = now();
        db.insertSeq(compressed).get();
        tinsert = now();

        time_load += diff_to_sec(&tbeg,&tload);
        time_compress += diff_to_sec(&tload,&tcompress);
        time_insert += diff_to_sec(&tcompress,&tinsert);
#else
        Future<std::vector<char> > img = disk.loadImageSeq(path);
        Future<std::vector<char> > compressed = compressor.compressSeq(img, COMPRESS_WIDTH, COMPRESS_HEIGHT, true, COMPRESS_BIT_PER_PIXEL);
        db.insertSeq(compressed).release();
#endif
        ++nimg;
    }

    if (is.bad()) {
        std::cout << "IO error" << std::endl;
    }

    t1 = now();

    db.saveDb(db_dir).get();

    t2 = now();

    double time_calls = diff_to_sec(&t0,&t1);
    double time_process = diff_to_sec(&t0,&t2);

    if (measure_path != NULL)
    {
        std::string logpath(measure_path);
        logpath += "_main";
        std::ofstream os(logpath, std::ofstream::out | std::ofstream::app);
        if (!os.is_open())
        {
            std::cout << "Could not write measure to path " << measure_path << std::endl;
        }
        else
        {
#if MEASURE_SEQUENCES
            time_load /= double(nimg);
            time_compress /= double(nimg);
            time_insert /= double(nimg);

            os << nimg << std::scientific << "\t" << time_calls << "\t" << time_process << "\t" << time_load << "\t" << time_compress << "\t" << time_insert << "\n";
#else
            os << nimg << std::scientific << "\t" << time_calls << "\t" << time_process << "\n";
#endif
            os.close();
        }

    }

}

std::vector<int> assignProcs(int offset, int p)
{
    std::vector<int> pids(p);
    for (int i = 0; i < p; ++i)
    {
        pids[i] = offset + i;
    }

    return pids;
}

int main(int argc, char** argv)
{
    activebsp_init();

    if (argc < 5)
    {
        printf("Invalid number of arguments\n");
        help(argv[0]);
        activebsp_finalize();
        return 1;
    }

    int disk_p       = atoi(argv[1]);
    int compressor_p = atoi(argv[2]);
    int db_p         = atoi(argv[3]);

    std::vector<int> disk_pids       = assignProcs(1,disk_p);
    std::vector<int> compressor_pids = assignProcs(disk_p + 1,compressor_p);
    std::vector<int> db_pids         = assignProcs(disk_p + compressor_p + 1,db_p);

    Proxy<ImgStore> disk = createActiveObject<ImgStore>(disk_pids);
    Proxy<MonochromeCompressor> compressor = createActiveObject<MonochromeCompressor>(compressor_pids);
    Proxy<ImageDatabase> db = createActiveObject<ImageDatabase>(db_pids);

    const char * cmd = argv[4];
    if (strcmp(cmd, "store") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for store cmd\n");
            help(argv[0]);
            help_store();
        }
        else
        {
            char * src_local = argv[5];
            char * dst_base = argv[6];

            storeImageOnDisk(disk, src_local, dst_base);
        }
    }
    else if (strcmp(cmd, "store_dir") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for store_dir cmd\n");
            help(argv[0]);
            help_store_dir();
        }
        else
        {
            char * src_dir = argv[5];
            char * dst_base = argv[6];

            storeImagesOnDisk(disk, src_dir, dst_base);
        }

    }
    else if (strcmp(cmd, "load") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for load cmd\n");
            help(argv[0]);
            help_load();
        }
        else
        {
            char * src_base = argv[5];
            char * dst = argv[6];

            loadImg(disk, src_base, dst);
        }

    }
    else if (strcmp(cmd, "compress") == 0)
    {
        if (argc < 9)
        {
            printf("Invalid number of arguments for compress cmd\n");
            help(argv[0]);
            help_compress();
        }
        else
        {
            char * src_base = argv[5];
            char * dst = argv[6];
            int width = atoi(argv[7]);
            int height = atoi(argv[8]);

            compressImg(disk, compressor, src_base, dst, width, height);
        }

    }
    else if (strcmp(cmd, "fill_db") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for compress cmd\n");
            help(argv[0]);
            help_fillDb();
        }
        else
        {
            char * img_dir = argv[5];
            char * db_path = argv[6];

            fillDb(disk, compressor, db, img_dir, db_path);
        }

    }
    else if (strcmp(cmd, "search_img") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for search_img cmd\n");
            help(argv[0]);
            help_search_img();
        }
        else
        {
            char * img_path = argv[5];
            char * db_path = argv[6];

            searchImg(disk, compressor, db, img_path, db_path);
        }

    }
    else if (strcmp(cmd, "store_ppm_dir") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for search_img cmd\n");
            help(argv[0]);
            help_store_ppm_dir();
        }
        else
        {
            char * img_dir = argv[5];
            char * disk_dir = argv[6];

            storePPMDirOnDisk(disk, img_dir, disk_dir);
        }

    }
    else if (strcmp(cmd, "insert_list") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for search_img cmd\n");
            help(argv[0]);
            help_store_ppm_dir();
        }
        else
        {
            char * img_list_file = argv[5];
            char * db_dir = argv[6];

            char * measure_path = (argc >= 8) ? argv[7] : NULL;

            insertDir(disk, compressor, db, img_list_file, db_dir, measure_path);
        }

    }
    else if (strcmp(cmd, "insert_list_seq") == 0)
    {
        if (argc < 7)
        {
            printf("Invalid number of arguments for search_img cmd\n");
            help(argv[0]);
            help_store_ppm_dir();
        }
        else
        {
            char * img_list_file = argv[5];
            char * db_dir = argv[6];

            char * measure_path = (argc >= 8) ? argv[7] : NULL;

            insertDirSeq(disk, compressor, db, img_list_file, db_dir, measure_path);
        }

    }
    else
    {
        printf("cmd not recognized : %s\n", cmd);
    }

    disk.destroyObject();
    compressor.destroyObject();
    db.destroyObject();

    activebsp_finalize();

    return 0;
}

