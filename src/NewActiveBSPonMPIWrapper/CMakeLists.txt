set(LIB_NAME newactivebsponmpiwrapper)

set(BSPLIB_WRAPPER_LIB ${LIB_NAME} PARENT_SCOPE)


#set(BSPLIB_WRAPPER_LIB ${LIB_NAME} PARENT_SCOPE)

set (BSPONMPI_ENABLE_TESTING OFF)

add_subdirectory(bsponmpi)

file(GLOB SRC
    "include/*.h"
    "include/*.hpp"
    "src/*.cpp"
)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/bsponmpi/include)

set (CMAKE_CXX_STANDARD 11)

add_library(${LIB_NAME}
    ${SRC}
)

target_include_directories(${LIB_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src
)

target_link_libraries(${LIB_NAME}
    ${MPI_C_LIBRARIES}
    ${BSPLIB_LIB}
    bsponmpi
    activebsp-common
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
