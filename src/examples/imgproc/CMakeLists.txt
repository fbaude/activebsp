set (EXE_NAME test_imgproc)

# Define an executable
add_executable(${EXE_NAME}
    src/main.cpp
    src/ImgStore.h
    src/ImgStore.cpp
    src/MonochromeCompressor.h
    src/MonochromeCompressor.cpp
    src/ImageDatabase.h
    src/ImageDatabase.cpp
    src/div_range.h
    src/div_range.cpp
    src/measure.h
    src/measure.cpp)

add_executable(gen_images
    src/gen_images.cpp
)

#add_executable(imgproc_mpi
#    src/imgproc_mpi.cpp
#    src/div_range.cpp
#    src/pipe_redistribution.h
#    src/pipe_redistribution.cpp
#)

#target_link_libraries(imgproc_mpi
#    ${MPI_C_LIBRARIES}
#)

find_package(JPEG REQUIRED)

# Define the libraries this project depends upon
target_link_libraries(${EXE_NAME}
    ${ACTIVEBSP_LIB}
    ${JPEG_LIBRARIES}
)

set(GTEST_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/googletest/googletest/include")
set(GTEST_MAIN gtest_main)
set(GTEST_LIB gtest)

include_directories(${GTEST_INCLUDE_DIR})

set(test_name test_redistribution)
#add_executable(${test_name} src/${test_name}.cpp src/pipe_redistribution.cpp src/div_range.cpp)
#target_link_libraries(${test_name}
#      ${GTEST_LIB}
#      ${GTEST_MAIN}
#      ${MPI_C_LIBRARIES}
#      ${ACTIVEBSP_LIB}
#)

#add_test(NAME ${test_name} COMMAND ./${test_name})
#set_tests_properties(${test_name} PROPERTIES TIMEOUT 5)

#set(test_name test_redistribution_comm)
#add_executable(${test_name} src/${test_name}.cpp src/div_range.cpp src/pipe_redistribution.cpp)
#target_link_libraries(${test_name}
#      ${GTEST_LIB}
#      ${GTEST_MAIN}
#      ${MPI_C_LIBRARIES}
#      ${MPI_C_LIBRARIES}
#      ${ACTIVEBSP_LIB}
#)

#add_test(NAME ${test_name} COMMAND mpirun -n 8 ./${test_name})
#set_tests_properties(${test_name} PROPERTIES TIMEOUT 5)

