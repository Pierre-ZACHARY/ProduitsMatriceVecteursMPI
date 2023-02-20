#include <mpi.h>
#include <iostream>
#include <vector>

using namespace std;

vector<int> job_queue(vector<int>& data) {
    int rank, world_size, nworkers;
    const int root = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
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

int main(int argc, char** argv){
    vector<int> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    job_queue(data);
    return 0;
}