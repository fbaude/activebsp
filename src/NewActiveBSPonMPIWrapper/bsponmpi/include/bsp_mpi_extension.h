#ifndef BSP_MPI_EXTENSION_H
#define BSP_MPI_EXTENSION_H

#include <mpi.h>

#include "bsp.h"

#ifdef __cplusplus
extern "C" {
#endif

DLL_PUBLIC void bsp_use_world_subset( MPI_Comm subset ) ;



#ifdef __cplusplus
}
#endif

#endif // BSP_MPI_EXTENSION_H
