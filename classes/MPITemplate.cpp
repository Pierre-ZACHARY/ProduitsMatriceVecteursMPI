//
// Created by Yukiix on 09/02/2023.
//

#include <cstdio>
#include <iostream>
#include "MPITemplate.h"
using namespace std;
MPITemplate::MPITemplate(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    cout << "Hello from process " << world_rank << " out of " << world_size << " processes" << endl;
}

MPITemplate::~MPITemplate() {

    MPI_Finalize();
}

