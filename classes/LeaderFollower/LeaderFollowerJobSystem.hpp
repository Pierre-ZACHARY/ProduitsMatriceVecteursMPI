//
// Created by Pierre Zachary on 20/02/2023.
//

#ifndef PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERJOBSYSTEM_HPP
#define PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERJOBSYSTEM_HPP


#include "LeaderFollowerTemplate.hpp"
#include "MessagePayload.hpp"
#include <vector>

class LeaderFollowerJobSystem : public LeaderFollowerTemplate {
private:
    void listener();
    void sender(std::vector<MessagePayload>& payloads);
    int numTasksCompleted = 0;

    MPI_Comm termination_comm; // send a message to this communicator to terminate the listener
    MPI_Request send(int rank, MessagePayload payload);

    MessagePayload receive(int rank);
public:
    virtual void run(size_t payloadSize);

    explicit LeaderFollowerJobSystem(int nworkers);
    ~LeaderFollowerJobSystem();

protected:
    virtual std::vector<MessagePayload> getInitialData(); // vector of messages to send to workers
    virtual void onListenerInit();
    virtual MessagePayload onTaskReceived(MessagePayload payload);
    virtual void onTaskCompleted(int rank, MessagePayload payload);
    virtual void onAllTasksCompleted();

    size_t payloadSize;
};



#endif //PRODUITSMATRICEVECTEURSMPI_LEADERFOLLOWERJOBSYSTEM_HPP
