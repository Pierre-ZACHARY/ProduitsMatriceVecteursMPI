#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "MPI2_Impl.hpp"
#include "Matrix.h"
#include "MPI1_Impl.h"

using namespace std;

void seq(int argc, char** argv){
    MPI1_Impl mpi1(argc, argv);
    if(mpi1.world_rank == 0) cout << "Seq Impl :" << endl;
    vector<int> sizes = {256, 512, 1024};
    int numrun = 3;
    for(int i = 0; i < sizes.size(); i++) {
        if(mpi1.world_rank == 0) cout << "mean times for size " << sizes[i] << " with 25% zeros" << endl;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        int number_of_lines_with_zeros = ceil(sizes[i] *0.25);
        for(int j = 0; j < numrun; j++){
            Matrix<float> a = Matrix<float>(sizes[i], sizes[i]);
            a.fill(2.0f);
            Matrix<float> zeros = Matrix<float>(number_of_lines_with_zeros, sizes[i]);
            zeros.fill(0.0f);
            a.setSubMatrix(sizes[i]/2-number_of_lines_with_zeros/2, sizes[i]/2-number_of_lines_with_zeros/2 + number_of_lines_with_zeros, zeros);
            Matrix<float> b = Matrix<float>(sizes[i], sizes[i]);
            b.fill(6.0f);
            if(mpi1.world_rank == 0) auto c = a * b;
        }
        end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end - start;
        if(mpi1.world_rank == 0) cout << elapsed_seconds.count()/numrun<< "s" << endl;
        if(mpi1.world_rank == 0) cout <<  "mean times for size " << sizes[i] << " with 50% zeros" << endl;
        start = chrono::system_clock::now();
        number_of_lines_with_zeros = ceil(sizes[i] *0.5);
        for(int j = 0; j < numrun; j++){
            Matrix<float> a = Matrix<float>(sizes[i], sizes[i]);
            a.fill(2.0f);
            Matrix<float> zeros = Matrix<float>(number_of_lines_with_zeros, sizes[i]);
            a.setSubMatrix(sizes[i]/2-number_of_lines_with_zeros/2, sizes[i]/2-number_of_lines_with_zeros/2 + number_of_lines_with_zeros, zeros);
            Matrix<float> b = Matrix<float>(sizes[i], sizes[i]);
            b.fill(6.0f);
            if(mpi1.world_rank == 0) auto c = a * b;
        }
        end = chrono::system_clock::now();
        elapsed_seconds = end - start;
        if(mpi1.world_rank == 0) cout << elapsed_seconds.count()/numrun<< "s" << endl;
    }
    if(mpi1.world_rank == 0) cout << "\n------------------------------------\n" << endl;

}

void impl2(int argc, char** argv){
    MPI2_Impl mpi2(argc, argv);
    if(mpi2.world_rank == 0) cout << "MPI2 Impl :" << endl;
    vector<int> sizes = {256, 512, 1024};
    int numrun = 10;
    for(int i = 0; i < sizes.size(); i++){
        if(mpi2.world_rank == 0) cout <<  "mean times for size " << sizes[i] << " with 25% zeros" << endl;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        for(int j = 0; j < numrun; j++)
            mpi2.run(sizes[i],  ceil(sizes[i]*0.25));
        end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end - start;
        if(mpi2.world_rank == 0) cout << elapsed_seconds.count()/numrun << endl;

        if(mpi2.world_rank == 0) cout <<  "mean times for size " << sizes[i] << " with 50% zeros" << endl;
        start = chrono::system_clock::now();
        for(int j = 0; j < numrun; j++)
            mpi2.run(sizes[i], ceil(sizes[i]*0.50));
        end = chrono::system_clock::now();
        elapsed_seconds = end - start;
        if(mpi2.world_rank == 0) cout << elapsed_seconds.count()/numrun<< "s" << endl;
    }
    if(mpi2.world_rank == 0) cout << "\n------------------------------------\n" << endl;
}

void impl1(int argc, char** argv){
    MPI1_Impl mpi1(argc, argv);
    if(mpi1.world_rank == 0)  cout << "MPI1 Impl :" << endl;
    vector<int> sizes = {256, 512, 1024};
    int numrun = 10;
    for(int i = 0; i < sizes.size(); i++){
        if(mpi1.world_rank == 0) cout <<  "mean times for size " << sizes[i] << " with 25% zeros" << endl;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        for(int j = 0; j < numrun; j++)
            mpi1.run(sizes[i], ceil(sizes[i] * 0.25));
        end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end - start;
        if(mpi1.world_rank == 0) cout << elapsed_seconds.count()/numrun << endl;

        if(mpi1.world_rank == 0) cout <<  "mean times for size " << sizes[i] << " with 50% zeros" << endl;
        start = chrono::system_clock::now();
        for(int j = 0; j < numrun; j++)
            mpi1.run(sizes[i], ceil(sizes[i] * 0.50));
        end = chrono::system_clock::now();
        elapsed_seconds = end - start;
        if(mpi1.world_rank == 0) cout << elapsed_seconds.count()/numrun<< "s" << endl;
    }
    if(mpi1.world_rank == 0) cout << "\n------------------------------------\n" << endl;
}

int main(int argc, char** argv){
    MPI2_Impl mpi2(argc, argv);
//    seq(argc, argv);
//    impl1(argc, argv);
    impl2(argc, argv);
    return 0;
}