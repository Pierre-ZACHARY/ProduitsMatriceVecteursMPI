//
// Created by Yukiix on 09/02/2023.
//

#include <cstdio>
#include "MPITemplate.h"

MPITemplate::MPITemplate(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
}

MPITemplate::~MPITemplate() {
    MPI_Finalize();
}

void MPITemplate::run() {

}
