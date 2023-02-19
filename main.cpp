


#include <cstdio>
#include "MPI1_Impl.h"
#include "Matrix.h"

int main(int argc, char** argv) {
    int* size= new int[3]{256, 512, 1024};
    // seq run
    for(int i = 0; i < 3; i++) {
        Matrix<float> A(size[i], size[i]);
        Matrix<float> B(size[i], size[i]);
        A.fill(2.0f);
        B.fill(6.0f);
        std::chrono::time_point<std::chrono::system_clock> start, end;
        start = std::chrono::system_clock::now();
        for(int j = 0; j < 3; j++){
            Matrix<float> C = A * B;
        }
        end = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsed_seconds = end - start;
        printf("Seq run : Elapsed time for %d : %fs\n", size[i], elapsed_seconds.count());
    }


//    MPI1_Impl mpi1(argc, argv);
//    for(int i = 0; i < 3; i++) {
//        std::chrono::time_point<std::chrono::system_clock> start, end;
//        start = std::chrono::system_clock::now();
//        for(int j = 0; j < 10; j++){
//            mpi1.run(size[i]);
//        }
//        end = std::chrono::system_clock::now();
//        std::chrono::duration<double> elapsed_seconds = end - start;
//        printf("Elapsed time for %d : %fs\n", size[i], elapsed_seconds.count());
//    }

    return 0;
}