#include <mpi.h>
#include <cstdio>

class MPIExample {
public:
    MPIExample(int argc, char** argv) {
        MPI_Init(&argc, &argv);

        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);

        MPI_Win_create(&win_buf, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    }

    ~MPIExample() {
        MPI_Win_free(&win);
        MPI_Finalize();
    }

    void run() {
        int target_rank = (world_rank + 1) % world_size;
        if (world_rank == 0) {
            MPI_Win_lock(MPI_LOCK_EXCLUSIVE, target_rank, 0, win);
            win_buf = 42;
            MPI_Win_unlock(target_rank, win);
        }
        else {
            MPI_Win_lock(MPI_LOCK_SHARED, target_rank, 0, win);
            printf("Rank %d received %d from rank %d\n", world_rank, win_buf, target_rank);
            MPI_Win_unlock(target_rank, win);
        }
    }

private:
    MPI_Win win;
    int world_rank, world_size;
    int win_buf;
};

int main(int argc, char** argv) {
    MPIExample example(argc, argv);
    example.run();

    return 0;
}