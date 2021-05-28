
set(GTEST_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/thirdparty/googletest/googletest/include")
set(GTEST_MAIN gtest_main)
set(GTEST_LIB gtest)

include_directories(${GTEST_INCLUDE_DIR})

function(create_unit_test test_name)

    add_executable(${test_name} ${test_name}.cpp)
    target_link_libraries(${test_name}
      ${GTEST_LIB}
      ${GTEST_MAIN}
      ${ACTIVEBSP_LIB}
    )

    add_test(NAME ${test_name} COMMAND ./${test_name} --gtest_output=xml:${CMAKE_BINARY_DIR}/test_output/unit/${test_name}.xml)
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 5)
    set_tests_properties(${test_name} PROPERTIES LABELS unit)

endfunction(create_unit_test)


create_unit_test(test_unit_ThreadedQueue)
create_unit_test(test_unit_CreateActorMessage)
create_unit_test(test_unit_CallActorMessage)
create_unit_test(test_unit_serialization)
create_unit_test(test_unit_contiguous_serialization)
create_unit_test(test_unit_DistrVector)
create_unit_test(test_unit_contiguous_assign)

