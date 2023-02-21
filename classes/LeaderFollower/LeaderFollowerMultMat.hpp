//
// Created by Pierre Zachary on 21/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERMULTMAT_HPP
#define PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERMULTMAT_HPP

#include "LeaderFollowerJobSystem.hpp"
#include "Matrix.h"


class LeaderFollowerMultMat: public LeaderFollowerJobSystem{
private:
    int size, numZeroLines, numLinesPerTask;
    Matrix<float> a,b,c;
    MessagePayload serializePayload(int line_index, float* line);
    void deserializePayload(MessagePayload payload, int* line_index, float* line);
protected:
    std::vector<MessagePayload> getInitialData() override;
    void onListenerInit() override;
    MessagePayload onTaskReceived(MessagePayload payload) override;
    void onTaskCompleted(int rank, MessagePayload payload) override;
    void onAllTasksCompleted() override;

public:
    LeaderFollowerMultMat(int nworkers): LeaderFollowerJobSystem(nworkers) {}
    ~LeaderFollowerMultMat();

    void run(int size, int numZeroLines, int numLinesPerTask);
};


#endif //PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERMULTMAT_HPP
