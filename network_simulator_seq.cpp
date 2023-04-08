#include "common.h"
#include "timing.h"
#include <iostream>

void simulateStep(std::vector<People> population){

}


int main(int argc, char *argv[]) {
    int pid;
    int nproc;
    MPI_Status status;
    MPI_Comm comm = MPI_COMM_WORLD;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // Get total number of processes specificed at start of run
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    StartupOptions options = parseOptions(argc, argv);
    std::vector<People> population;
    if (pid == 0) {
        loadFromFile(options.inputFile, population);
    }
    //TO DO: fill in the steps
    for (int i = 0; i < options.numIterations; i++) {
        simulateStep(population);

    }

    if(pid == 0){
        saveToFile(options.outputFile, population);
    }

}