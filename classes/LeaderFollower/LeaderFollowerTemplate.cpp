//
// Created by Pierre Zachary on 20/02/2023.
//

#include <cstdio>
#include <omp.h>
#include "LeaderFollowerTemplate.hpp"
#include <iostream>
using namespace std;

void LeaderFollowerTemplate::master() {
    MPI_Comm_spawn("./LeaderFollower", MPI_ARGV_NULL, nworkers, MPI_INFO_NULL, 0, MPI_COMM_SELF, &worker_comm, MPI_ERRCODES_IGNORE);
    merge(worker_comm, &merged_rank, &merged_size);
    printf("Master process (global rank %d / local rank %d) has spawned %d worker processes.\n", merged_rank, rank, nworkers);
}

void LeaderFollowerTemplate::worker() {
    merge(parent_comm, &merged_rank, &merged_size);
    printf("Worker process (global rank %d / local rank %d) has been spawned.\n", merged_rank, rank);
}

void LeaderFollowerTemplate::merge(MPI_Comm comm, int *_merged_rank, int *_merged_size) {
    if(isMaster) MPI_Intercomm_merge(comm, 0, &merged_comm);
    else         MPI_Intercomm_merge(comm, 1, &merged_comm);
    MPI_Comm_rank(merged_comm, _merged_rank);
    MPI_Comm_size(merged_comm, _merged_size);
}


LeaderFollowerTemplate::~LeaderFollowerTemplate() {
    if(isMaster) MPI_Comm_disconnect(&worker_comm);
    else         MPI_Comm_disconnect(&parent_comm);
    MPI_Comm_free(&merged_comm);
    MPI_Finalize();
}

int LeaderFollowerTemplate::getRank() const {
    return rank;
}

int LeaderFollowerTemplate::getSize() const {
    return size;
}

int LeaderFollowerTemplate::getMergedRank() const {
    return merged_rank;
}

int LeaderFollowerTemplate::getMergedSize() const {
    return merged_size;
}

bool LeaderFollowerTemplate::isMaster1() const {
    return isMaster;
}

int LeaderFollowerTemplate::getNworkers() const {
    return nworkers;
}

std::string LeaderFollowerTemplate::toString() const {
    string s;
    if(isMaster1()) s = "Master " + to_string(getMergedRank()) + "/" + to_string(getRank());
    else s = "Worker " + to_string(getMergedRank()) + "/" + to_string(getRank());
    return s;
}

LeaderFollowerTemplate::LeaderFollowerTemplate(int nworkers): nworkers(nworkers) {
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_get_parent(&parent_comm);
    isMaster = (parent_comm == MPI_COMM_NULL);
    if (isMaster) master();
    else worker();
}
