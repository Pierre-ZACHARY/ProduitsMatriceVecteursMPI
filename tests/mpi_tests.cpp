//
// Created by Yukiix on 09/02/2023.
//
#include <gtest/gtest.h>
#include "classes/MPI1_Impl.h"

TEST(MPITests, BasicAssertions) {
    // initialize argv with argv[0] = "1"
    char** argv = new char*[0];
    MPI1_Impl mpi1(1, argv);
}