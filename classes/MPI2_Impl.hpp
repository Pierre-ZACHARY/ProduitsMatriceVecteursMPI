//
// Created by Pierre Zachary on 19/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MPI2_IMPL_HPP
#define PRODUITSMATRICEVECTEURSMPI_MPI2_IMPL_HPP


#include "MPITemplate.h"

class MPI2_Impl: public MPITemplate {
private :
    int* shared_buffer = new int[1]{100};
    MPI_Win shared_buffer_win;
    MPI_Win matrixA_buffer_win;
    MPI_Win matrixB_buffer_win;
    MPI_Win matrixC_buffer_win;
public:
    MPI2_Impl(int argc, char** argv);
    ~MPI2_Impl();

    int nextTask(int rows_per_tasks);
    void run(int size, int rows_per_tasks, int number_of_lines_with_zeros);
};


#endif //PRODUITSMATRICEVECTEURSMPI_MPI2_IMPL_HPP
