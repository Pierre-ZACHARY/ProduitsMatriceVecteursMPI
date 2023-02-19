cmake_minimum_required(VERSION 3.16)
project(ProduitsMatriceVecteursMPI)
set(CMAKE_CXX_STANDARD 20)
include_directories(classes)

# Find MPI
Find_Package(MPI REQUIRED)
include_directories(${MSMPI_INC})
include_directories(${MPI_INCLUDE_PATH})

# Find OpenMP
Find_Package(OpenMP REQUIRED)

# Set compile flags for MPI and OpenMP
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${MPI_COMPILE_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${MPI_LINK_FLAGS}")


add_executable(ProduitsMatriceVecteursMPI main.cpp
        classes/MPITemplate.cpp
        classes/MPITemplate.h
        classes/Matrix.cpp
        classes/Matrix.h
        classes/MPI1_Impl.cpp
        classes/MPI1_Impl.h)
add_executable(Follower follower.cpp
        classes/MPITemplate.cpp
        classes/MPITemplate.h
        classes/Matrix.cpp
        classes/Matrix.h
        classes/MPI1_Impl.cpp
        classes/MPI1_Impl.h)
target_link_libraries(ProduitsMatriceVecteursMPI MPI::MPI_CXX OpenMP::OpenMP_CXX)
target_link_libraries(Follower MPI::MPI_CXX OpenMP::OpenMP_CXX)

# Tests
enable_testing()
add_subdirectory(tests)