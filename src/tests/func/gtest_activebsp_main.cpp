#include <gtest/gtest.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "activebsp.h"
#include "activebsp_init_config.h"

#define GTEST_OUTPUT_PATTERN "--gtest_output="

char process_output[PATH_MAX];

using namespace std;

void append_pid_gtest(int * argc, char *** argv)
{
    int s,p;
    MPI_Comm_rank(MPI_COMM_WORLD, &s);
    MPI_Comm_rank(MPI_COMM_WORLD, &p);

    // Try to find gtest file option
    int found_pos =-1;
    char * gtest_option = NULL;
    for (int i = 0; i < *argc; ++i)
    {
        if (strncmp((*argv)[i], GTEST_OUTPUT_PATTERN, strlen(GTEST_OUTPUT_PATTERN)) == 0)
        {
            found_pos = i;
            gtest_option = (*argv)[found_pos];
            break;
        }
    }

    // Check whether everyone or noone found it
    int min_pos, max_pos;
    MPI_Allreduce(&found_pos, &min_pos, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
    MPI_Allreduce(&found_pos, &max_pos, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);

    // If noone specified the option, continue without it
    if (min_pos == -1 && max_pos == -1) return;

    char recv_buf[PATH_MAX];

    // If not everyone have the option, use process 0's value
    if (min_pos == -1)
    {
        if (s == 0)
        {
            // only accept process 0's value
            if (found_pos == -1)
            {
                MPI_Abort(MPI_COMM_WORLD, 1);
            }

            // copy it into some common variable buffer
            strncpy(recv_buf, gtest_option, PATH_MAX);
        }

        // Do the broadcast
        MPI_Bcast(recv_buf, PATH_MAX, MPI_CHAR, 0, MPI_COMM_WORLD);
        gtest_option = recv_buf;
    }

    // Add a new arg with the new parameter
    if (found_pos == -1)
    {
        char ** new_argv = (char**) malloc((*argc + 2) * sizeof(char*));

        memcpy(&new_argv, argv, (*argc) * sizeof(char**));

        found_pos = *argc;
        new_argv[found_pos] = recv_buf;

        new_argv[*argc + 1] = NULL;

        ++(*argc);
        *argv = new_argv;
    }

    // Find the extension position if there is one
    int arglen = strlen(gtest_option);

    int extpos = -1;
    for (int j = arglen - 1; j >= 0; --j)
    {
        if (gtest_option[j] == '.')
        {
            extpos = j;
            break;
        }

    }

    // If none, consider empty string extension
    if (extpos == -1)
    {
        extpos = arglen;
    }

    // Build a string with the pid between the base and extension
    strncpy(process_output, gtest_option, extpos);
    sprintf(process_output + extpos, "_%d%s", s, gtest_option + extpos);

    // Write the string to replace the old option
    (*argv)[found_pos] = process_output;
}

int main(int argc, char* argv[])
{
    int provided, required = ACTIVEBSP_MPI_THREAD_REQUIRED;

    MPI_Init_thread(&argc, &argv, required, &provided);

    if (provided < required)
    {
        printf("MPI thread implementation has insufficiant thread support for activebsp\n");
        return 1;
    }

    append_pid_gtest(&argc, &argv);

    ::testing::InitGoogleTest(&argc, argv);

    activebsp::activebsp_init(&argc, &argv);

    int rc = RUN_ALL_TESTS();

    activebsp::activebsp_finalize();

    return rc;
}
