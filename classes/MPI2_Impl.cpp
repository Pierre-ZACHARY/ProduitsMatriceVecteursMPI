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
    MPI_Win_allocate_shared(sizeof(int), 1, MPI_INFO_NULL, MPI_COMM_WORLD, &shared_buffer, &shared_buffer_win);
}

void MPI2_Impl::run(int size, int rows_per_tasks, int number_of_lines_with_zeros) {
    Matrix<float> m1 = Matrix<float>(size, size);
    Matrix<float> m2 = Matrix<float>(size, size);
    Matrix<float> m3 = Matrix<float>(size, size);
    vector<int> tasks = vector<int>();
    MPI_Win_create(m1.data(), size * size * sizeof(float), sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,
                   &matrixA_buffer_win);
    MPI_Win_create(m3.data(), size * size * sizeof(float), sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,
                   &matrixC_buffer_win);
    if (world_rank == 0) {
        m1.fill(2.0f);
        Matrix<float> zeros = Matrix<float>(number_of_lines_with_zeros, size);
        m1.setSubMatrix(size/2-number_of_lines_with_zeros/2, size/2-number_of_lines_with_zeros/2 + number_of_lines_with_zeros, zeros);
        m2.fill(6.0f);
        MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, shared_buffer_win);
        shared_buffer[0] = size;
        MPI_Win_unlock(0, shared_buffer_win);
        MPI_Win_create(m2.data(), size * size * sizeof(float), sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD,
                       &matrixB_buffer_win);
    }
    else {
        MPI_Win_create(nullptr, 0, sizeof(float), MPI_INFO_NULL, MPI_COMM_WORLD, &matrixB_buffer_win);
    }
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, matrixB_buffer_win);
    MPI_Get(m2.data(), size*size, MPI_FLOAT, 0, 0, size*size, MPI_FLOAT, matrixB_buffer_win);
    MPI_Win_unlock(0, matrixB_buffer_win);
    dout << "Process " << world_rank << " has received matrix B \n"<< endl;


    MPI_Request* request = new MPI_Request[size];
    MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, matrixA_buffer_win);
    for(int i = 0; i < size; i++) {
        MPI_Rget(m1.data()+i*size, size, MPI_FLOAT, 0, i*size,  size, MPI_FLOAT, matrixA_buffer_win,
                 &request[i]);
    }
    MPI_Win_unlock(0, matrixA_buffer_win);

    int next = nextTask(rows_per_tasks);
    while(next != -1) {
        int start = next - rows_per_tasks;
        for(int i = start; i < next; i++) {
            dout << "Process " << world_rank << " is waiting for task " << i << endl;
            MPI_Wait(&request[i], MPI_STATUS_IGNORE);
        }
        Matrix<float> m1_local = m1.subMatrix(start, next);
        Matrix<float> m3_local = m1_local * m2;
        m3.setSubMatrix(start, next, m3_local);
        tasks.push_back(next);
        next = nextTask(rows_per_tasks);
        MPI_Win_fence(0, matrixA_buffer_win);
    }
    end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    cout << "Process "<< world_rank << " has done : "<< tasks.size() << " tasks in " << elapsed_seconds.count() << " seconds" << endl;
    MPI_Win number_of_tasks_done_win;
    int* number_of_tasks_done = new int[1]{(int) tasks.size()};
    MPI_Win_create(number_of_tasks_done, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD,
                   &number_of_tasks_done_win);
    MPI_Win tasks_win;
    MPI_Win_create(tasks.data(), tasks.size() * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD,
                   &tasks_win);
    MPI_Win_fence(0, tasks_win);
    MPI_Win_fence(0, number_of_tasks_done_win);
    //retrieves the results
    if(world_rank == 0) {
        for(int i = 1; i < world_size; i++) {
            int* number_of_tasks_done_local = new int[1];
            MPI_Win_lock(MPI_LOCK_SHARED, i, 0, number_of_tasks_done_win);
            MPI_Get(number_of_tasks_done_local, 1, MPI_INT, i, 0, 1, MPI_INT, number_of_tasks_done_win);
            MPI_Win_unlock(i, number_of_tasks_done_win);
            dout << "Root : "<< *number_of_tasks_done_local << " tasks done by process " << i << endl;
            if(*number_of_tasks_done_local == 0) continue;
            int* tasks_local = new int[*number_of_tasks_done_local];
            MPI_Win_lock(MPI_LOCK_SHARED, i, 0, tasks_win);
            MPI_Get(tasks_local, *number_of_tasks_done, MPI_INT, i, 0, *number_of_tasks_done, MPI_INT, tasks_win);
            MPI_Win_unlock(i, tasks_win);
            for(int j = 0; j < *number_of_tasks_done; j++) {
                int start = tasks_local[j] - rows_per_tasks;
                dout << "Root : "<< "retrieving task " << tasks_local[j] << " from process " << i << " starting line : "<< start << endl;
                MPI_Win_lock(MPI_LOCK_SHARED, i, 0, matrixC_buffer_win);
                MPI_Get(m3.data()+start*size, rows_per_tasks*size, MPI_FLOAT, i, start*size, rows_per_tasks*size, MPI_FLOAT, matrixC_buffer_win);
                MPI_Win_unlock(i, matrixC_buffer_win);
            }
        }
//        dout << "Root m3 : \n" << m3 << endl;
        dout << "Root : "<< m3(size-1, size-1) << endl;
        ASSERT_EQ(m3(size-1, size-1), 12.0*size);
    }
    else{
        dout << "Process "<< world_rank << " m3 = \n"<< m3 << endl;
        dout << "Process "<< world_rank << " tasks = \n" << endl;
        for(int i = 0; i < tasks.size(); i++) {
            dout << tasks[i] << " ";
        }
        dout<<endl;

    }
    MPI_Win_free(&matrixA_buffer_win);
    MPI_Win_free(&matrixB_buffer_win);
    MPI_Win_free(&matrixC_buffer_win);
    MPI_Win_free(&tasks_win);
    MPI_Win_free(&number_of_tasks_done_win);
    delete[] request;

}

int MPI2_Impl::nextTask(int rows_per_tasks) {
    dout << "Process " << world_rank << " is looking for a task" << endl;
    MPI_Win_lock(MPI_LOCK_EXCLUSIVE, 0, 0, shared_buffer_win);
    int next = shared_buffer[-(world_rank)];
    if(next == 0) {
        MPI_Win_unlock(0, shared_buffer_win);
        return -1;
    }
    shared_buffer[-(world_rank)] = next-rows_per_tasks;
    MPI_Win_unlock(0, shared_buffer_win);
    return next;
}

MPI2_Impl::~MPI2_Impl() {
    MPI_Win_free(&shared_buffer_win);
}


