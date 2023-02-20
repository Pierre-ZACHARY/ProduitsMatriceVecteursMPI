//
// Created by Yukiix on 09/02/2023.
//

#include <cstdio>
#include <iostream>
#include <fstream>
#include "MPITemplate.h"
using namespace std;

bool MPITemplate::is_initialized = false;
int MPITemplate::numInstances = 0;

MPITemplate::MPITemplate(int argc, char** argv) {
    if(!is_initialized) {
        MPI_Init(&argc, &argv);
        is_initialized = true;
    }
    numInstances++;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
//    cout << "Hello from process " << world_rank << " out of " << world_size << " processes" << endl;
}

MPITemplate::~MPITemplate() {
    numInstances--;
    if(numInstances == 0) MPI_Finalize();
}

