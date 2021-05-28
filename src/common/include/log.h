#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <unistd.h>

#include <mpi.h>

#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#define IS_MASTER() (getpid() == syscall(SYS_gettid))
#define LOG_THREAD_LETTER() (IS_MASTER() ? "M" : "W")

#define LOG_ERROR(fmt, ...) \
do {  \
    int rank; \
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
    fprintf(stderr, "%s:%d:%s(): P%d%s: ERROR: " fmt "\n", __FILE__, __LINE__, __func__, rank, LOG_THREAD_LETTER(), __VA_ARGS__); \
} while (0)

#if ENABLE_LOG_TRACE

#define LOG_TRACE(fmt, ...) \
do {  \
    int rank; \
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
    fprintf(stdout, "%s:%d:%s(): P%d%s: " fmt "\n", __FILE__, __LINE__, __func__, rank, LOG_THREAD_LETTER(), __VA_ARGS__); \
} while (0)

#else

#define LOG_TRACE(fmt, ...)

#endif

#if ENABLE_LOG_MEASURE

#include <time.h>

#include "measure.h"

#define START_MEASURE(num) \
struct timespec __log_measure_t_start_ ## num ; \
struct timespec __log_measure_t_end_ ## num ; \
double __log_measure_t ## num; \
__log_measure_t_start_ ## num = activebsp::now();


#define END_MEASURE(num) \
__log_measure_t_end_ ## num = activebsp::now(); \
__log_measure_t ## num = diff_to_sec(&__log_measure_t_start_ ## num, &__log_measure_t_end_ ## num);

#define GET_MEASURE(num) __log_measure_t ## num

#define LOG_MEASURE(num, name) \
do {  \
int rank; \
MPI_Comm_rank(MPI_COMM_WORLD, &rank); \
printf("P%d%s: %s took %.10e s\n", rank, LOG_THREAD_LETTER(), name, GET_MEASURE(num)); \
} \
while (0)


#else
#define START_MEASURE(num)
#define END_MEASURE(num)
#define GET_MEASURE(num)
#define LOG_MEASURE(num, name)


#endif

#endif // LOG_H
