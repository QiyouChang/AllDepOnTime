#include "common.h"
#include "timing.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <string>

//hyperparameters
std::string inputFile = "./benchmark_files/sample1.txt";
std::string outputFile = "./seqOutputv1.txt";
std::string outputResult = "./seqResultv1.txt";
int numIterations = 5;
float discount = 0.9;


std::vector<People> simulateStep(std::vector<People> &population){
    // Sequential Implementation that simply computes all weights of people 
    // and try adding them together
    int total = population.size();
    
    //Iterate all people within the population and sum up all of likes and update it to others
    for(size_t i = 0; i < total; i ++){
        People person = population[i];
        std::vector<Connection> connections = person.conn;
        float change = 0.f;
        for(Connection individual: connections){
            change += individual.like;
        }
        population[i].eval += change;
    }
    return population;
}

int main(int argc, char *argv[]) {
    int pid;
    int nproc;
    // Initialize MPI
    MPI_Init(&argc, &argv);
    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // Get total number of processes specificed at start of run
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
   
    std::vector<People> population;
    if (pid == 0) {
        loadFromFile(inputFile, population);
    }

    Timer totalSimulationTimer;
    for (int i = 0; i < numIterations; i++) {
        population = simulateStep(population);
    }

    if(pid == 0){
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("total simulation time: %.6fs\n", totalSimulationTime);
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }
    MPI_Finalize();
}