#include <mpi.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "MPI2_Impl.hpp"
#include "Matrix.h"
#include "MPI1_Impl.h"

using namespace std;

vector<int> job_queue(vector<int>& data) {
    int rank, world_size, nworkers;
    const int root = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    cout << "Rank: " << rank << " Size: " << world_size << endl;
    nworkers = world_size - 1;

    MPI_Barrier(MPI_COMM_WORLD);
    int N = data.size();
    vector<int> new_data(N);
    int send_mesg, recv_mesg;

    if (rank == root) { // I am root
        int idx_recv = 0;
        int idx_sent = 0;

        // Array of workers requests
        MPI_Request sreqs_workers[nworkers];
        // -1 = start, 0 = channel not available, 1 = channel available
        int status_workers[nworkers];
        fill_n(status_workers, nworkers, -1);

        // Send message to workers
        for (int dst = 1; dst <= nworkers && idx_sent < N; dst++) {
            send_mesg = data[idx_sent];
            MPI_Isend(&send_mesg, 1, MPI_INT, dst, dst + 32, MPI_COMM_WORLD, &sreqs_workers[dst - 1]);
            idx_sent++;
            status_workers[dst - 1] = 0;
            cout << "Root: Sent number " << send_mesg << " to Worker " << dst << endl;
        }

        // Send and receive messages until all elements are added
        while (idx_recv != N) {
            // Check to see if there is an available message to receive
            for (int dst = 1; dst <= nworkers; dst++) {
                if (status_workers[dst - 1] == 0) {
                    int flag;
                    MPI_Test(&sreqs_workers[dst - 1], &flag, MPI_STATUS_IGNORE);
                    if (flag) {
                        status_workers[dst - 1] = 1;
                    }
                }
            }
            for (int dst = 1; dst <= nworkers; dst++) {
                if (status_workers[dst - 1] == 1) {
                    int ismessage;
                    MPI_Iprobe(dst, dst + 32, MPI_COMM_WORLD, &ismessage, MPI_STATUS_IGNORE);
                    if (ismessage) {
                        // Receives message
                        MPI_Recv(&recv_mesg, 1, MPI_INT, dst, dst + 32, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        idx_recv++;
                        new_data[idx_recv - 1] = recv_mesg;
                        cout << "Root: Received number " << recv_mesg << " from Worker " << dst << endl;
                        if (idx_sent < N) {
                            send_mesg = data[idx_sent];
                            // Sends new message
                            MPI_Isend(&send_mesg, 1, MPI_INT, dst, dst + 32, MPI_COMM_WORLD, &sreqs_workers[dst - 1]);
                            idx_sent++;
                            status_workers[dst - 1] = 1;
                            cout << "Root: Sent number " << send_mesg << " to Worker " << dst << endl;
                        }
                    }
                }
            }
        }

        for (int dst = 1; dst <= nworkers; dst++) {
            // Termination message to worker
            send_mesg = -1;
            MPI_Isend(&send_mesg, 1, MPI_INT, dst, dst + 32, MPI_COMM_WORLD, &sreqs_workers[dst - 1]);
            status_workers[dst - 1] = 0;
            cout << "Root: Sent termination message to Worker " << dst << endl;
        }

        MPI_Waitall(nworkers, sreqs_workers, MPI_STATUSES_IGNORE);
        cout << "Root: All workers terminated : " << endl;
        // print newdata
        for (int i = 0; i < N; i++) {
            cout << new_data[i] << " ";
        }
        cout << endl;
    } else { // I am worker
        int status_worker = -1; // -1 = start, 0 = channel not available, 1 = channel available
        while (true) {
            MPI_Request sreq_worker;
            int ismessage;
            MPI_Iprobe(root, rank + 32, MPI_COMM_WORLD, &ismessage, MPI_STATUS_IGNORE);
            if (ismessage) {
                // Receives message
                MPI_Recv(&recv_mesg, 1, MPI_INT, root, rank + 32, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                if (recv_mesg == -1) {
                    cout << "Worker " << rank << ": Received termination message" << endl;
                    break;
                }
                cout << "Worker " << rank << ": Received number " << recv_mesg << endl;
                // Sends message
                send_mesg = recv_mesg + 100;
                MPI_Isend(&send_mesg, 1, MPI_INT, root, rank + 32, MPI_COMM_WORLD, &sreq_worker);
                cout << "Worker " << rank << ": Sent number " << send_mesg << endl;
                status_worker = 0;
            }

            if (status_worker == 0) {
                int flag;
                MPI_Test(&sreq_worker, &flag, MPI_STATUS_IGNORE);
                if (flag) {
                    status_worker = 1;
                }
            }
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return new_data;
}

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