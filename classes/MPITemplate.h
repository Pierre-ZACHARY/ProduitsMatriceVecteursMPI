//
// Created by Yukiix on 09/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MPITEMPLATE_H
#define PRODUITSMATRICEVECTEURSMPI_MPITEMPLATE_H


#include <mpi.h>

class MPITemplate {
    protected:
        int world_rank, world_size;

    public:
        MPITemplate(int argc, char** argv);
        ~MPITemplate();
        virtual void run();
};


#endif //PRODUITSMATRICEVECTEURSMPI_MPITEMPLATE_H
