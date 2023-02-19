//
// Created by Yukiix on 09/02/2023.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include "MPI1_Impl.h"
#include "Matrix.h"

using namespace std;


MPI1_Impl::MPI1_Impl(int argc, char **argv) : MPITemplate(argc, argv) {
    if (world_rank == 0) {
        cout << "MPI1_Impl" << endl;
    }
}


void MPI1_Impl::run(int size) {
    Matrix<float> m1 = Matrix<float>(size, size);
    Matrix<float> m2 = Matrix<float>(size, size);
    if (world_rank == 0) {
        m1.fill(2.0f);
        m2.fill(6.0f);
    }


    // Broadcast the second matrix to all other processes
    MPI_Bcast(m2.data(), m2.size(), MPI_FLOAT, 0, MPI_COMM_WORLD);


    // scatterv the first matrix to all other processes
    int * rowSendCounts = new int[world_size];
    int * sendcounts = new int[world_size];
    int * displs = new int[world_size];
    int displ = 0;
    for(int i = 0; i < world_size; i++) {
        rowSendCounts[i] = m1.rows() / world_size;
        if(i < m1.rows() % world_size) rowSendCounts[i]++;
        sendcounts[i] = rowSendCounts[i] * m1.cols();
        displs[i] = displ;
        displ += sendcounts[i];
    }
    Matrix<float> m1_local(rowSendCounts[world_rank], m1.cols());
    MPI_Scatterv(m1.data(), sendcounts, displs, MPI_FLOAT, m1_local.data(), sendcounts[world_rank], MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Compute the local result
    Matrix<float> m3_local = m1_local * m2;

    // Gather the local results
    Matrix<float> m3(m1.rows(), m2.cols());
    MPI_Gatherv(m3_local.data(), m3_local.size(), MPI_FLOAT, m3.data(), sendcounts, displs, MPI_FLOAT, 0, MPI_COMM_WORLD);

}


MPI1_Impl::~MPI1_Impl() = default;


