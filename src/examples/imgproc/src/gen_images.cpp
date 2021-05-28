#include <iostream>
#include <fstream>
#include <random>

#include <stdlib.h>
#include <string.h>

#include <linux/limits.h>

using namespace std;

void help(char * name)
{
    cout << "Usage : " << name << " n out_dir width height method methodparams..." << std::endl;
}

void help_rand()
{
    cout << "rand cmd parameters : density" << endl;
}

void help_rand_square()
{
    cout << "rand_square cmd parameters : none" << endl;
}

typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pix_t;

size_t slashpath(char * path)
{
    int path_len = strlen(path);
    if (path[path_len - 1] != '/')
    {
        path[path_len++] = '/';
        path[path_len] = '\0';
    }

    return path_len;
}

size_t write_ppm_header(char * buf, int height, int width)
{
    return sprintf(buf, "P6 %d %d 255\n", width, height);
}

size_t init_ppm(char ** buf, size_t * buf_size, int height, int width)
{
    unsigned char header_buf[1024];
    size_t header_size = write_ppm_header((char *) header_buf, width, height);
    size_t npix = width * height;
    size_t pixmap_size = npix * sizeof(pix_t);
    *buf_size  = header_size + pixmap_size;

    *buf = (char *) malloc(*buf_size);
    memcpy(*buf, header_buf, header_size);

    return header_size;
}

void gen_rand(const char * out_dir, int n, int height, int width, int)
{
    char path[PATH_MAX];
    strcpy(path, out_dir);
    int path_len = slashpath(path);

    char * img;
    size_t img_size;
    size_t header_size = init_ppm(&img, &img_size, height, width);
    size_t npix = height * width;
    pix_t * pixmap = (pix_t *) &img[header_size];

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0,255);

    for (int i = 0; i < n; ++i)
    {
        for (size_t j = 0; j < npix; ++j)
        {
            pixmap[j].r = dist(rng);
            pixmap[j].g = dist(rng);
            pixmap[j].b = dist(rng);
        }

        sprintf(path + path_len, "img_gen_%d.ppm",i);
        ofstream os(path);
        if (!os.is_open())
        {
            cout << "could not open file " << path << " for writing" << endl;
        }
        os.write(img, img_size);
        os.close();
    }

    free(img);
}


void gen_rand_square(const char * out_dir, int n, int height, int width)
{
    char path[PATH_MAX];
    strcpy(path, out_dir);
    int path_len = slashpath(path);

    char * img;
    size_t img_size;
    size_t header_size = init_ppm(&img, &img_size, height, width);
    size_t npix = height * width;
    pix_t * pixmap = (pix_t *) &img[header_size];

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> x_dist(0,width);
    std::uniform_int_distribution<std::mt19937::result_type> y_dist(0,height);

    pix_t white {0,0,0};
    pix_t black {255,255,255};

    for (int i = 0; i < n; ++i)
    {
        size_t square_x1 = x_dist(rng);
        size_t square_x2 = x_dist(rng);
        size_t square_y1 = y_dist(rng);
        size_t square_y2 = y_dist(rng);

        if (square_x1 > square_x2)
        {
            std::swap(square_x1,square_x2);
        }
        if (square_y1 > square_y2)
        {
            std::swap(square_y1,square_y2);
        }

        for (size_t j = 0; j < npix; ++j)
        {
            size_t x = j / height;
            size_t y = j % height;

            if (x >= square_x1 && x <= square_x2 && y >= square_y1 && y <= square_y2)
            {
                pixmap[j] = white;
            }
            else
            {
                pixmap[j] = black;
            }
        }

        sprintf(path + path_len, "img_gen_%d.ppm",i);
        ofstream os(path);
        if (!os.is_open())
        {
            cout << "could not open file " << path << " for writing" << endl;
        }
        os.write(img, img_size);
        os.close();
    }

    free(img);
}

int main (int argc, char ** argv)
{
    if (argc < 6)
    {
        help(argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    char * out_dir = argv[2];
    int width = atoi(argv[3]);
    int height = atoi(argv[4]);
    char * method = argv[5];

    if (strcmp(method, "rand") == 0)
    {
        if (argc < 7)
        {
            help_rand();
            return 1;
        }

        int density = atoi(argv[6]);
        if (density < 0 || density > 100)
        {
            cout << "density must be between 0 and 100" << endl;
            return 1;
        }

        gen_rand(out_dir, n, height, width, density);
    }
    else if (strcmp(method, "rand_square") == 0)
    {
        if (argc < 6)
        {
            help_rand_square();
            return 1;
        }

        gen_rand_square(out_dir, n, height, width);
    }
    else
    {
        cout << "method " << method << " not recognized" << endl;
        return 1;
    }

    return 0;
}
