cmake_minimum_required(VERSION 3.16)
project(ProduitsMatriceVecteursMPI)
set(CMAKE_CXX_STANDARD 20)
include_directories(classes)
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(APPLE)
    set(OpenMP_C_FLAGS "-Xclang -fopenmp -I /usr/local/Cellar/libomp/15.0.7/include -L /usr/local/Cellar/libomp/15.0.7/lib")
    set(OpenMP_C_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY omp)
    set(OpenMP_CXX_FLAGS "-Xclang -fopenmp -I /usr/local/Cellar/libomp/15.0.7/include -L /usr/local/Cellar/libomp/15.0.7/lib")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY omp)
endif()

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
        classes/MPI1_Impl.h classes/MPI2_Impl.cpp classes/MPI2_Impl.hpp)
add_executable(LeaderFollower leader_follower.cpp
        classes/Matrix.cpp
        classes/Matrix.h
        classes/LeaderFollower/LeaderFollowerTemplate.cpp
        classes/LeaderFollower/LeaderFollowerTemplate.hpp
        classes/LeaderFollower/LeaderFollowerJobSystem.cpp
        classes/LeaderFollower/LeaderFollowerJobSystem.hpp
        classes/LeaderFollower/MessagePayload.hpp
        classes/LeaderFollower/LeaderFollowerMultMat.cpp
        classes/LeaderFollower/LeaderFollowerMultMat.hpp)
target_link_libraries(ProduitsMatriceVecteursMPI MPI::MPI_CXX OpenMP::OpenMP_CXX GTest::gtest_main)
target_link_libraries(LeaderFollower MPI::MPI_CXX OpenMP::OpenMP_CXX GTest::gtest_main)

# Tests
enable_testing()
add_subdirectory(tests)