set (EXE_NAME test_innerproduct)

# Define an executable
add_executable(${EXE_NAME}
    src/bspedupack.c
    src/InnerProductActor.h
    src/InnerProductActor.cpp
    src/main.cpp
)

# Define the libraries this project depends upon
target_link_libraries(${EXE_NAME}
    ${ACTIVEBSP_LIB}
)
