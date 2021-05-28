set (EXE_NAME bench_relayvector)

# Define an executable
add_executable(${EXE_NAME}
    src/relayvector.cpp
)

# Define the libraries this project depends upon
target_link_libraries(${EXE_NAME}
    ${ACTIVEBSP_LIB}
)
