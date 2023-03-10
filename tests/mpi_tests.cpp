//
// Created by Yukiix on 09/02/2023.
//
#include <gtest/gtest.h>
#include "classes/MPI1_Impl.h"
#include "MPI2_Impl.hpp"
#include "LeaderFollower/LeaderFollowerMultMat.hpp"

TEST(MPITests, BasicAssertions) {
    // initialize argv with argv[0] = "1"
    char** argv = new char*[0];

    MPI2_Impl mpi2(1, argv);
    MPI1_Impl mpi1(1, argv);
//    LeaderFollowerMultMat lfmm(1); // doesn't work on github actions because not enough cores to spawn workers
    mpi1.run(100, 10);
    mpi2.run(100, 10);
//    lfmm.run(100, 10);
}