#include <iostream>
#include <fstream>
#include <vector>

#include <stdlib.h>
#include <string.h>

#include <linux/limits.h>

#include <mpi.h>

#include "div_range.h"
#include "pipe_redistribution.h"

using namespace std;

void usage(char * pname)
{
    cout << "Usage : " << pname << " diskprocs compressorprocs dbprocs imglistpath" << endl;
}

int path_src, compressor_head, compressor_nprocs;


void disk_func(MPI_Comm comm, int path_src_pid, MPI_Comm out_comm)
{
    int s,p,out_p;
    int * out_zeros, *out_snd_meta, *out_snd_sizes, *out_snd_dspls;
    MPI_Comm_rank(comm, &s);
    MPI_Comm_size(comm, &p);

    char base[PATH_MAX];
    char dir_path[PATH_MAX];
    MPI_Status status;
    int nbytes,base_len;

    MPI_Comm_remote_size(out_comm, &out_p);


    out_snd_meta = (int *) malloc(2 * out_p * sizeof(int));

    out_snd_sizes = out_snd_meta;
    out_snd_dspls = out_snd_meta + out_p;
    out_zeros = (int *) malloc(out_p * sizeof(int));
    for (int i = 0; i < out_p; ++i)
    {
        out_zeros[i] = 0;
    }

    std::vector<char> part;

    while (true)
    {
        if (s == 0)
        {
            MPI_Recv(base, PATH_MAX, MPI_CHAR, path_src_pid, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_CHAR, &nbytes);

            if (nbytes == 0)
            {
                base[0] = '\0';
            }
        }

        MPI_Bcast(base, PATH_MAX, MPI_CHAR, 0, comm);

        if ((base_len = strlen(base)) == 0) break;

        sprintf(base + base_len, "_%d", s);

        std::ifstream is(base);
        if (!is.is_open())
        {
            std::cout << "p" << s << " Failed to open file : " << base << std::endl;
            return;
        }

        is.seekg(0, std::ios_base::end);
        size_t block_size = is.tellg();
        is.seekg(0, std::ios_base::beg);

        int max_block_size = 0;
        int total_size = 0;

        MPI_Allreduce(&block_size, &max_block_size, 1, MPI_INT, MPI_MAX, comm);
        MPI_Allreduce(&block_size, &total_size, 1, MPI_INT, MPI_SUM, comm);

        part.resize(max_block_size);
        is.read(&part[0], block_size);

        parallel_pipe_send(&part[0], total_size, s, p, out_p, out_snd_dspls, out_snd_sizes, out_zeros, out_comm);
    }

    cout << "compressor finished with the sendings " << endl;

    for (int i = 0; i < out_p; ++i)
    {
        out_snd_sizes[i] = 0;
    }
    MPI_Alltoall(out_snd_sizes, 1, MPI_INT, NULL, 0, MPI_INT, out_comm);

}

void compressor_func(MPI_Comm comm, MPI_Comm in_comm, MPI_Comm out_comm)
{
    int s,p,in_p;
    int * in_zeros, * in_rcv_sizes, *in_rcv_dspls, *in_rcv_meta;
    MPI_Comm_rank(comm, &s);
    MPI_Comm_size(comm, &p);
    MPI_Comm_remote_size(in_comm, &in_p);

    std::vector<char> part,tb_part;
    size_t part_size = 0;



    in_rcv_meta = (int *) malloc(2 * in_p * sizeof(int));
    in_rcv_sizes = in_rcv_meta;
    in_rcv_dspls = in_rcv_meta + in_p;
    in_zeros = (int *) malloc(in_p * sizeof(int));
    for (int i = 0; i < in_p; ++i)
    {
        in_zeros[i] = 0;
    }
    bool qstop = true;

    while (true)
    {
        qstop = -1 == parallel_pipe_recv(part, s, p, in_rcv_dspls, in_rcv_sizes, in_zeros, in_comm);

        if (qstop)
        {
            break;
        }

        //redistribute_out_comm(&part[0], 64, s, p);
    }


}

void db_func(MPI_Comm comm)
{
    int s,p;
    MPI_Comm_rank(comm, &s);
    MPI_Comm_size(comm, &p);

    cout << "Hello from db " << s << "/" << p << endl;
}

void unused_func(MPI_Comm comm)
{
    int s,p;
    MPI_Comm_rank(comm, &s);
    MPI_Comm_size(comm, &p);

    cout << "Hello from unused " << s << "/" << p << endl;
}

typedef enum
{
    root,
    disk,
    compressor,
    db,
    unknown
} split_color_t;

void create_intercomms(split_color_t color, MPI_Comm local_comm, int disk_head, int compressor_head, int db_head, MPI_Comm * in_intercomm, MPI_Comm * out_intercomm)
{
    /* Build inter-communicators. Tags are hard-coded. */
    if (color == disk)
    {
    /* Group 0 communicates with group 1. */
    MPI_Intercomm_create( local_comm, 0, MPI_COMM_WORLD, compressor_head, 1, out_intercomm);
    }
    else if (color == compressor)
    {
        /* Group 1 communicates with groups 0 and 2. */
        MPI_Intercomm_create( local_comm, 0, MPI_COMM_WORLD, disk_head, 1, in_intercomm);
        MPI_Intercomm_create( local_comm, 0, MPI_COMM_WORLD, db_head, 12, out_intercomm);
    }
    else if (color == db)
    {
        MPI_Intercomm_create( local_comm, 0, MPI_COMM_WORLD, compressor_head, 12, in_intercomm);
    }

    /* Group 2 communicates with group 1. */
}

int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);

    int s,p;
    MPI_Comm_rank(MPI_COMM_WORLD, &s);
    MPI_Comm_size(MPI_COMM_WORLD, &p);

    int disk_procs,compressor_procs,db_procs;
    char * imglist;

    if (s == 0)
    {
        if (argc != 5)
        {
            cout << "Invalid number of arguments" << endl;
            MPI_Abort(MPI_COMM_WORLD, 1);
        }

        disk_procs = atoi(argv[1]);
        compressor_procs = atoi(argv[2]);
        db_procs = atoi(argv[3]);

        imglist = argv[4];
    }

    MPI_Bcast(&disk_procs, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&compressor_procs, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&db_procs, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int disk_head = 1;
    int compressor_head = disk_procs + disk_head;
    int db_head = compressor_head + compressor_procs;
    int db_end = db_head + db_procs;

    split_color_t color = s == 0 ? root : s < compressor_head ? disk : s < db_head ? compressor : s < db_end ? db : unknown;

    MPI_Comm group_comm, in_intercomm, out_intercomm;
    MPI_Comm_split(MPI_COMM_WORLD, color, s, &group_comm);

    create_intercomms(color, group_comm, disk_head, compressor_head, db_head, &in_intercomm, &out_intercomm);

    switch(color)
    {
    case disk:
        disk_func(group_comm, 0, out_intercomm);
        break;
    case compressor:
        compressor_func(group_comm, in_intercomm, out_intercomm);
        break;
    case db:
        db_func(group_comm);
        break;
    case unknown:
        unused_func(group_comm);
        break;
    case root:
        break;
    default:
        cout << "Error : unknown assignment to process " << s << endl;
        MPI_Abort(MPI_COMM_WORLD, 2);
    }

    if (s == 0)
    {
        ifstream is(imglist);
        if (!is.is_open())
        {
            cout << "Could not open path list file : " << imglist << endl;
            MPI_Abort(MPI_COMM_WORLD, 3);
        }

        string path;
        while (getline(is,path))
        {
            MPI_Send(path.c_str(), path.size() + 1, MPI_CHAR, disk_head, 0, MPI_COMM_WORLD);
        }

        MPI_Send(NULL, 0, MPI_CHAR, disk_head, 0, MPI_COMM_WORLD);

    }

    MPI_Finalize();
}
