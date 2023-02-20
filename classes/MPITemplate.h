//
// Created by Yukiix on 09/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_MPITEMPLATE_H
#define PRODUITSMATRICEVECTEURSMPI_MPITEMPLATE_H


#include <mpi.h>
#include <chrono>

class MPITemplate {
public:
    static bool is_initialized;
    static int numInstances;

protected:
    int world_rank, world_size;

public:
        MPITemplate(int argc, char** argv);
        ~MPITemplate();
};


#endif //PRODUITSMATRICEVECTEURSMPI_MPITEMPLATE_H
