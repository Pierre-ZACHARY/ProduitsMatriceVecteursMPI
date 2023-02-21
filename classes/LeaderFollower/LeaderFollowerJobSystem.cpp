//
// Created by Pierre Zachary on 20/02/2023.
//

#include "LeaderFollowerJobSystem.hpp"
#include <iostream>
using namespace std;


MPI_Request LeaderFollowerJobSystem::send(int rank, MessagePayload payload) {
    MPI_Request req;
    MPI_Isend(payload.data, payload.size, MPI_BYTE, rank, 0, merged_comm, &req);
    return req;
}

MessagePayload LeaderFollowerJobSystem::receive(int rank) {
    unsigned char* data = new unsigned char[payloadSize];
    MPI_Recv(data,payloadSize, MPI_BYTE, rank, 0, merged_comm, MPI_STATUS_IGNORE);
    MessagePayload payload(data, payloadSize);
    return payload;
}


void LeaderFollowerJobSystem::sender(vector<MessagePayload>& payloads) {

    int N = payloads.size();
    int idx_recv = 0;
    int idx_sent = 0;
    int nworkers = 1;
    // Array of workers requests
    MPI_Request sreqs_workers[nworkers];
    // -1 = start, 0 = channel not available, 1 = channel available
    int status_workers[nworkers];
    fill_n(status_workers, nworkers, -1);

    // Initial Send message to workers
    cout << *this << " Sending to " << nworkers << " workers" << endl;
    for (int dst = 1; dst <= nworkers && idx_sent < N; dst++) {
        cout << *this << " Sending task:"<<idx_sent<<" to " << dst << endl;
        sreqs_workers[dst - 1] = send(dst, payloads[idx_sent]);
        idx_sent++;
        status_workers[dst - 1] = 0;
    }

    // Send and receive messages until all elements are added
    while (idx_recv != N) {
        // Check to see if there is an available message to receive
        for (int dst = 1; dst <= nworkers; dst++) {
            if (status_workers[dst - 1] == 0) {
                int flag;
                MPI_Test(&sreqs_workers[dst - 1], &flag, MPI_STATUS_IGNORE);
                if (flag) {
                    status_workers[dst - 1] = 1;
                }
            }
        }
        for (int dst = 1; dst <= nworkers; dst++) {
            if (status_workers[dst - 1] == 1) {
                int ismessage;
                MPI_Iprobe(dst, 0, merged_comm, &ismessage, MPI_STATUS_IGNORE);
                if (ismessage) {
                    // Receives message
                    MessagePayload payload = receive(dst);
                    // do something with the worker payload
                    onTaskCompleted(dst, payload);
                    idx_recv++;
                    if (idx_sent < N) {
                        // Sends new message
                        cout << *this << " Sending task:"<<idx_sent<<" to " << dst << endl;
                        sreqs_workers[dst - 1] = send(dst, payloads[idx_sent]);
                        idx_sent++;
                        status_workers[dst - 1] = 1;
                    }
                }
            }
        }
    }

    for (int dst = 1; dst <= nworkers; dst++) {
        // Termination message to worker
        int termination_message = -1;
        MPI_Isend(&termination_message, 1, MPI_INT, dst, 0, termination_comm, &sreqs_workers[dst - 1]);
        status_workers[dst - 1] = 0;
        cout << *this <<": Sent termination message to Worker " << dst << endl;
    }

    MPI_Waitall(nworkers, sreqs_workers, MPI_STATUSES_IGNORE);
    cout << *this <<": All workers terminated." << endl;
    onAllTasksCompleted();
}


void LeaderFollowerJobSystem::listener() {
    int status_worker = -1; // -1 = start, 0 = channel not available, 1 = channel available
    int root = 0;
    onListenerInit();
    while (true) {
        MPI_Request sreq_worker;
        int ismessage, termination_message;
        MPI_Iprobe(root, 0, merged_comm, &ismessage, MPI_STATUS_IGNORE);
        MPI_Iprobe(root, 0, termination_comm, &termination_message, MPI_STATUS_IGNORE);
        if (ismessage) {
            // Receives message
            MessagePayload recv_mesg = receive(root);
            // Do something with the message
            MessagePayload new_mesg = onTaskReceived(recv_mesg);
            // then tell the master that the task is completed
            sreq_worker = send(root, new_mesg);
            status_worker = 0;
        }
        if(termination_message){
            int recv_mesg;
            MPI_Recv(&recv_mesg, 1, MPI_INT, root, 0, termination_comm, MPI_STATUS_IGNORE);
            if (recv_mesg == -1) {
                cout << *this << ": Received termination message" << endl;
                break;
            }
            else{
                cerr << *this << ": Received unknown message" << endl;
            }
        }

        if (status_worker == 0) {
            int flag;
            MPI_Test(&sreq_worker, &flag, MPI_STATUS_IGNORE);
            if (flag) {
                status_worker = 1;
            }
        }
    }
}


void LeaderFollowerJobSystem::onAllTasksCompleted() {
    // do something when all tasks are completed and received on master ( workers already terminated )
}

std::vector<MessagePayload> LeaderFollowerJobSystem::getInitialData() {
    // this function should be overridden by the user so that it returns the initial data to send to the workers
    return vector<MessagePayload>();
}


void LeaderFollowerJobSystem::run(size_t payloadSize) {
    this->payloadSize = payloadSize;
    if(isMaster1()){
        vector<MessagePayload> data = getInitialData();
        sender(data);
    } else {
        listener();
    }
}

LeaderFollowerJobSystem::~LeaderFollowerJobSystem() {
    MPI_Comm_free(&termination_comm);
}

LeaderFollowerJobSystem::LeaderFollowerJobSystem(int nworkers): LeaderFollowerTemplate(nworkers) {
    MPI_Comm_dup(merged_comm, &termination_comm);
}

void LeaderFollowerJobSystem::onListenerInit() {
    // do something when the listener is initialized
}


void LeaderFollowerJobSystem::onTaskCompleted(int rank, MessagePayload payload) {

}

MessagePayload LeaderFollowerJobSystem::onTaskReceived(MessagePayload payload) {
    // do something with the payload
    return payload;
}