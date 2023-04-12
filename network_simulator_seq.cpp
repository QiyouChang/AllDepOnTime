#include "common.h"
#include "timing.h"
#include <iostream>

void simulateStep(std::vector<People> population){
    // Sequential Implementation that simply computes all weights of people 
    // and try adding them together
    int total = population.size();

    //Iterate all people within the population and sum up all of likes and update it to others
    for (People person:population){
        std::vector<Connection> connections = person.conn;
        float change = 0.f;
        for(Connection individual: connections){
            change += individual.like;
        }
        person.eval += change;
    }
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