//
// Created by Pierre Zachary on 21/02/2023.
//

#include "LeaderFollowerMultMat.hpp"
using namespace std;


vector<MessagePayload> LeaderFollowerMultMat::getInitialData() {
    a = Matrix<float>(size, size);
    b = Matrix<float>(size, size);
    c = Matrix<float>(size, size);
    a.fill(2.0f);
    Matrix<float> zeros = Matrix<float>(numZeroLines, size);
    zeros.fill(0.0f);
    a.setSubMatrix(size/2-numZeroLines/2, size/2+numZeroLines/2, zeros);
    b.fill(6.0f);
//    cout << *this << " : Created matrix a : \n"<< a <<endl;
//    cout << *this << " : Sending matrix b : \n"<< b <<endl;
    vector<MessagePayload> payloads = vector<MessagePayload>();
    // generate payloads
    for (int i = 0; i < size; i+=numLinesPerTask) {
        payloads.push_back(serializePayload(i, a.data()+i*size));
    }

    MPI_Bcast(b.data(), size*size, MPI_FLOAT, 0, merged_comm);
    return payloads;
}

void LeaderFollowerMultMat::onListenerInit() {
    auto *b_data = new float[size*size];

    MPI_Bcast(b_data, size*size, MPI_FLOAT, 0, merged_comm); // on doit récupérer b sur chaque worker
    b = Matrix<float>(size, size, b_data);
//    cout << *this << " : Received matrix b : \n"<< b <<endl;
    LeaderFollowerJobSystem::onListenerInit();
}

MessagePayload LeaderFollowerMultMat::onTaskReceived(MessagePayload payload) {
    // runs on worker
    float* line = new float[size*numLinesPerTask];
    int line_index;
    deserializePayload(payload, &line_index, line);
//    cout << *this << " : Received line " << line_index << " which contain : \n"<< line <<endl;
    Matrix<float> line_mat = Matrix<float>(numLinesPerTask, size, line);
//    cout << *this << " : Received line " << line_index << " which contain : \n"<< line_mat <<endl;
    Matrix<float> result = line_mat * b;
    return serializePayload(line_index, result.data());
}

void LeaderFollowerMultMat::onTaskCompleted(int rank, MessagePayload payload) {
    // run on master
    float* line = new float[size*numLinesPerTask];
    int line_index;
    deserializePayload(payload, &line_index, line);
    const Matrix<float> &linemat = Matrix<float>(numLinesPerTask, size, line);
//    cout << *this << " : Received result for line " << line_index << " which contain : \n"<< linemat <<endl;
    c.setSubMatrix(line_index, line_index + numLinesPerTask, linemat);
    LeaderFollowerJobSystem::onTaskCompleted(rank, payload);
}

void LeaderFollowerMultMat::onAllTasksCompleted() {
    // run on master
//    cout << "Result: \n" << c << endl;
    // verify last column/line has the good value

    float expected = 2.0f*6.0f*size;
    if(c(0,0) != expected) {
        cout << "Error: wrong result" << endl;
        exit(1);
    }
    LeaderFollowerJobSystem::onAllTasksCompleted();
}


void LeaderFollowerMultMat::run(int size, int numZeroLines, int numLinesPerTask) {
    if(numZeroLines>=size) {
        cout << "Error: numZeroLines must be less than size" << endl;
        exit(1);
    }
    this->size = size;
    this->numZeroLines = numZeroLines;
    this->numLinesPerTask = numLinesPerTask;
    this->payloadSize = sizeof(int)+size*sizeof(float)*numLinesPerTask;
    LeaderFollowerJobSystem::run(payloadSize);
}

LeaderFollowerMultMat::~LeaderFollowerMultMat() {
}

MessagePayload LeaderFollowerMultMat::serializePayload(int line_index, float *line) {
    unsigned char* result_payload = new unsigned char [payloadSize];
    memcpy(result_payload, &line_index, sizeof(int));
    memcpy(result_payload+sizeof(int), line, numLinesPerTask*size*sizeof(float));
    return {result_payload, payloadSize};
}

void LeaderFollowerMultMat::deserializePayload(MessagePayload payload, int *line_index, float *line) {
    // Copy the int value from the buffer
    memcpy(line_index, payload.data, sizeof(int));
//    cout << *this << " : Received line index " << *line_index << endl;
    // Copy the float value from the buffer after the int value
    memcpy(line, payload.data + sizeof(int), numLinesPerTask*size*sizeof(float));
//    cout << *this << " : Received line starting with " << *line << endl;
}
