set (EXE_NAME test_prefetch)

# Define an executable
add_executable(${EXE_NAME}
    src/main.cpp
)

# Define the libraries this project depends upon
target_link_libraries(${EXE_NAME}
    ${ACTIVEBSP_LIB}
)
