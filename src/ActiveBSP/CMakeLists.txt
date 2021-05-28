set (LIB_NAME activebsp)

file(GLOB ACTIVEBSP_SRC
    "include/*.h"
    "include/*.hpp"
    "src/*.cpp"
    "include/actormsg/*.h"
    "include/actormsg/*.hpp"
    "src/actormsg/*.cpp"
    "include/mpi/*.h"
    "include/mpi/*.hpp"
    "src/mpi/*.cpp"
    "include/future/*.h"
    "include/future/*.hpp"
    "src/future/*.cpp"
    "include/shmem/*.h"
    "include/shmem/*.hpp"
    "src/shmem/*.cpp"
    "include/worker/*.h"
    "include/worker/*.hpp"
    "src/worker/*.cpp"
    "include/management/*.h"
    "include/management/*.hpp"
    "src/management/*.cpp"
    "include/distributions/*.h"
    "include/distributions/*.hpp"
    "src/distributions/*.cpp"
    "include/procassign/*.h"
    "include/procassign/*.hpp"
    "src/procassign/*.cpp"
)

set (CMAKE_CXX_STANDARD 11)

include_directories(SYSTEM ${BSPLIB_WRAPPER_LIB}/include)
message("including ${BSPLIB_WRAPPER_LIB} ")

add_library(${LIB_NAME}
    ${ACTIVEBSP_SRC}
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${LIB_NAME} Threads::Threads)

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)

target_link_libraries(${LIB_NAME}
    ${MPI_C_LIBRARIES}
    ${Boost_LIBRARIES}
    ${BSPLIB_WRAPPER_LIB}
)

set(LIBRARY_CONFIG ${LIB_NAME}Config)

install(TARGETS ${LIB_NAME} EXPORT ${LIBRARY_CONFIG}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT ${LIBRARY_CONFIG} DESTINATION share/${LIB_NAME}/cmake)

# This makes the project importable from the build directory
export(TARGETS ${LIB_NAME} FILE ${LIBRARY_CONFIG}.cmake)
