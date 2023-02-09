//
// Created by Yukiix on 09/02/2023.
//

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include "MPI1_Impl.h"
#include "Matrice.h"

using namespace std;

void MPI1_Impl::run() {

}

MPI1_Impl::MPI1_Impl(int argc, char **argv) : MPITemplate(argc, argv) {
    int root = atoi(argv[0]);
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    cout << "root = " << root << " worldrank = "<< world_rank << " word_size = "<< world_size << endl;
    if(world_rank == root){
       // root will load the matrice, divide it equally and send it to the other processes
       Matrice m1(n, m);
       Matrice m2(n, m);
       // print m*m
    }
    else{
        // other processes will receive the matrice, compute the product and send it to the root
    }
}

MPI1_Impl::~MPI1_Impl() = default;


