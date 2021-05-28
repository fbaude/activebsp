set (EXE_NAME test_simplecall)

# Define an executable
add_executable(${EXE_NAME}
    src/ActorA.h
    src/ActorA.cpp
    src/main.cpp
)

# Define the libraries this project depends upon
target_link_libraries(${EXE_NAME}
    ${ACTIVEBSP_LIB}
)
