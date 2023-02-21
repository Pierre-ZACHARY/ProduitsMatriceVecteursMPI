#include "mpi.h"
#include "LeaderFollower/LeaderFollowerMultMat.hpp"
#include <vector>
#include <cmath>

using namespace std;
int main(int argc, char *argv[]){
    MPI_Init(&argc, &argv);
    LeaderFollowerMultMat lfmm(1);
    vector<int> sizes = {256, 512, 1024};
    int numrun = 10;
    int numLinesPerTasks = 2;
    for(int i = 0; i < sizes.size(); i++){
        cout <<  "mean times for size " << sizes[i] << " with 25% zeros" << endl;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        for(int j = 0; j < numrun; j++)
            lfmm.run(sizes[i],  ceil(sizes[i]*0.25), numLinesPerTasks);
        end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end - start;
        cout << elapsed_seconds.count()/numrun << endl;

        cout <<  "mean times for size " << sizes[i] << " with 50% zeros" << endl;
        start = chrono::system_clock::now();
        for(int j = 0; j < numrun; j++)
            lfmm.run(sizes[i], ceil(sizes[i]*0.50), numLinesPerTasks);
        end = chrono::system_clock::now();
        elapsed_seconds = end - start;
        cout << elapsed_seconds.count()/numrun<< "s" << endl;
    }
    lfmm.run(10, 2, 2);
    return 0;
}