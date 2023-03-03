//
// Created by Pierre Zachary on 20/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERTEMPLATE_HPP
#define PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERTEMPLATE_HPP


#include <mpi.h>
#include <ostream>
class LeaderFollowerTemplate {
protected:
    MPI_Comm parent_comm, worker_comm, merged_comm;
    void master();
    void worker();

    int merged_size;
private:
    int merged_rank;
    bool isMaster = false;
    int rank, size;
    int nworkers = 0;
    void merge(MPI_Comm comm, int* _merged_rank, int* _merged_size);
public:
    int getRank() const;
    int getNworkers() const;
    int getSize() const;
    int getMergedRank() const;
    int getMergedSize() const;
    bool isMaster1() const;
    explicit LeaderFollowerTemplate(int nworkers);
    ~LeaderFollowerTemplate();
    virtual std::string toString() const;
    friend std::ostream& operator<<(std::ostream& os, const LeaderFollowerTemplate& lf){
        return os<<lf.toString();
    }
};


#endif //PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERTEMPLATE_HPP
