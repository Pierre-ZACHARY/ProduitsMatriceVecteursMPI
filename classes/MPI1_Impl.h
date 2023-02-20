//
// Created by Yukiix on 09/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MPI1_IMPL_H
#define PRODUITSMATRICEVECTEURSMPI_MPI1_IMPL_H


#include "MPITemplate.h"


class MPI1_Impl: public MPITemplate{

    public:
        MPI1_Impl(int argc, char** argv);
        ~MPI1_Impl();

        void run(int size, int number_of_lines_with_zeros);
};


#endif //PRODUITSMATRICEVECTEURSMPI_MPI1_IMPL_H
