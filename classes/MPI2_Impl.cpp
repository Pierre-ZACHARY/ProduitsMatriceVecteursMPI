//
// Created by Pierre Zachary on 19/02/2023.
//

#include <iostream>
#include <gtest/gtest.h>
#include <fstream>
#include "MPI2_Impl.hpp"
#include "Matrix.h"

using namespace std;



#ifdef DEBUG
std::ostream &dout = dout;
#else
std::ofstream dev_null("/dev/null");
std::ostream &dout = dev_null;
#endif

MPI2_Impl::MPI2_Impl(int argc, char **argv) : MPITemplate(argc, argv) {
}

void MPI2_Impl::run(int size,  int number_of_lines_with_zeros) {
    Matrix<float> m2 = Matrix<float>(size, size);
    Matrix<float> m1, m3;
    vector<int> rows_per_process = vector<int>(world_size);
    vector<int> rows_per_process_offset = vector<int>(world_size);
    int offset = 0;
    for(int i = 0; i < world_size; i++) { // calcul effectuer sur chaque process / temps négligeable
        rows_per_process[i] = size / world_size;
        rows_per_process[i] += (i < size % world_size) ? 1 : 0;
        rows_per_process_offset[i] = offset;
        offset += rows_per_process[i];
    }
    int local_start = rows_per_process_offset[world_rank];
    int local_rows = rows_per_process[world_rank];
    int local_end = local_start + local_rows;
//    dout << "Process " << world_rank << " : " << local_start << " " << local_rows << " " << local_end << endl;
    if (world_rank == 0) {
        m3 = Matrix<float>(size, size);
        m1 = Matrix<float>(size, size);
        m1.fill(2.0f);
        Matrix<float> zeros = Matrix<float>(number_of_lines_with_zeros, size);
        zeros.fill(0.0f);
        m1.setSubMatrix(size/2-number_of_lines_with_zeros/2, size/2-number_of_lines_with_zeros/2 + number_of_lines_with_zeros, zeros);
        m2.fill(6.0f);
//        dout << "Root m1 : \n"<< m1<<endl;
//        dout << "Root m2 : \n"<< m2<<endl;
        MPI_Win_create(m2.data(), m2.size() * sizeof(float), sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,
                       &matrixB_buffer_win);
        MPI_Win_create(m1.data(), m1.size() * sizeof(float), sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,
                       &matrixA_buffer_win);
        MPI_Win_create(nullptr, 0, sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,
                       &matrixC_buffer_win);
    }
    else {
        m3 = Matrix<float>(local_rows, size);
        m1 = Matrix<float>(local_rows, size);
        MPI_Win_create(nullptr, 0, sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD, &matrixB_buffer_win);
        MPI_Win_create(nullptr, 0, sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,&matrixA_buffer_win);
        MPI_Win_create(m3.data(), local_rows*size*sizeof(float), sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,&matrixC_buffer_win);
    }
//    dout << "Process " << world_rank << " has created windows" << endl;


    MPI_Request requestM2;
    MPI_Request requestM1;
    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, matrixB_buffer_win);
    MPI_Rget(m2.data(), size*size, MPI_FLOAT, 0, 0, size*size, MPI_FLOAT, matrixB_buffer_win, &requestM2);
    MPI_Win_unlock(0, matrixB_buffer_win);

//    dout << "Process " << world_rank << " has sent its request to get matrix B" << endl;

    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, matrixA_buffer_win);
    MPI_Rget(m1.data(), local_rows*size, MPI_FLOAT, 0, local_start*size,  local_rows*size, MPI_FLOAT, matrixA_buffer_win,&requestM1);
    MPI_Win_unlock(0, matrixA_buffer_win);
//    dout << "Process " << world_rank << " has sent its request to get matrix A required lines" << endl;



    MPI_Wait(&requestM2, MPI_STATUS_IGNORE);
    MPI_Wait(&requestM1, MPI_STATUS_IGNORE);
//    dout << "Process " << world_rank << " has received matrix B : \n" << m2<< endl;
    for(int i = local_start; i < local_end; i++) {
        Matrix<float> m3_line = m1.getLine(i-local_start) * m2; // on multiplie la ligne i de A par B
        m3.setSubMatrix(i-local_start, i+1-local_start, m3_line); // on attribue cette ligne à m3 ( matrice résultat )
    }
//    dout << "Process " << world_rank << " has computed matrix C"<< endl;
//    dout << "Process matrix : \n"<< m3 << endl;

    // on attend que tout le monde ait fini de calculer sa partie de C
    MPI_Barrier(MPI_COMM_WORLD);
    if(world_rank==0){
        // root récupère les parties de m3 sur chaque processus
        MPI_Request* requestM3 = new MPI_Request[world_size-1];
        for(int i = 1; i < world_size; i++) {
            local_start = rows_per_process_offset[i];
            local_rows = rows_per_process[i];
            MPI_Win_lock(MPI_LOCK_SHARED, i, 0, matrixC_buffer_win);
            MPI_Rget(m3.data()+local_start*size, local_rows*size, MPI_FLOAT, i, 0, local_rows*size, MPI_FLOAT, matrixC_buffer_win, &requestM3[i-1]);
            MPI_Win_unlock(i, matrixC_buffer_win);
        }
        MPI_Waitall(world_size-1, requestM3, MPI_STATUSES_IGNORE);
        delete[] requestM3;
//        dout << "Root : \n"<< m3 << endl;
    }

    MPI_Win_free(&matrixA_buffer_win);
    MPI_Win_free(&matrixB_buffer_win);
    MPI_Win_free(&matrixC_buffer_win);
}


MPI2_Impl::~MPI2_Impl() = default;


