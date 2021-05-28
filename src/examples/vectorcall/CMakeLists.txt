set (EXE_NAME bench_vectorcall)

# Define an executable
add_executable(${EXE_NAME}
    src/vectorcall.cpp
)

# Define the libraries this project depends upon
target_link_libraries(${EXE_NAME}
    ${ACTIVEBSP_LIB}
)
