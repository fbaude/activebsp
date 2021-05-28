
set(GTEST_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/googletest/googletest/include")
set(GTEST_MAIN gtest_activebsp_main)
set(GTEST_LIB gtest)

include_directories(${GTEST_INCLUDE_DIR})

file(GLOB TEST_FILES ${PROJECT_SOURCE_DIR}/tests/func/test_*.cpp)

add_library(${GTEST_MAIN} STATIC gtest_activebsp_main.cpp)
target_link_libraries(${GTEST_MAIN}
    ${GTEST_LIB}
    ${MPI_LIBRARY}
    ${ACTIVEBSP_LIB}
)

function(create_func_test test_name nprocs)

    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name}
      ${GTEST_LIB}
      ${MPI_LIBRARY}
      ${GTEST_MAIN}
      ${ACTIVEBSP_LIB}
    )

    add_test(NAME ${test_name} COMMAND ${ABSPRUN_PATH} ${ABSP_RUNARGS} -n ${nprocs} ./${test_name} --gtest_output=xml:${CMAKE_BINARY_DIR}/test_output/func/${test_name}.xml )
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 5)
    set_tests_properties(${test_name} PROPERTIES LABELS func)

endfunction(create_func_test)

#create_func_test(test_func_bad_cleanup_one_process 2)
#create_func_test(test_func_bad_cleanup_multiple_processes 9)
create_func_test(test_func_simple_active_call 2)
create_func_test(test_func_simple_bsp_call 3)
create_func_test(test_func_multiple_call 2)
create_func_test(test_func_multiple_instanciate 3)
create_func_test(test_func_firstclass_future 3)
create_func_test(test_func_release_future 2)
create_func_test(test_func_seq_sync 3)
create_func_test(test_func_DistrVector 9)
create_func_test(test_func_distributionassoc 9)


