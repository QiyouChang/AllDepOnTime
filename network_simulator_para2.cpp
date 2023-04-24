#include "common.h"
#include "timing.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <string>
#include <cmath>
#include <set>

std::string inputFile = "./sample1.txt";
std::string outputFile = "./seqOutputv3.txt";
std::string outputResult = "./seqResultv3.txt";
#define numIterations 10
#define DR 0.9
#define Required_Deg 2

bool check_frontier(int *frontier, int length){
    for (int i = 0; i < length; i++){
        if (frontier[i]) return true;
    }
    return false;
}


std::vector<People> simulateStep(std::vector<People> &population, int pid, int nproc, int childsize){
    int total = population.size();
    MPI_Comm comm = MPI_COMM_WORLD;

    int displs[nproc];
    int recvcounts[nproc];
    std::fill_n(recvcounts, nproc, total);
    for (int i = 0; i < nproc; i++){
        displs[i] = (i == 0) ? 0 : recvcounts[i-1]+displs[i-1];
    }

    int displs2[nproc];
    int recvcounts2[nproc];
    std::fill_n(recvcounts, nproc, 1);
    for (int i = 0; i < nproc; i++){
        displs2[i] = (i == 0) ? 0 : recvcounts2[i-1]+displs2[i-1];
    }
    
    for (int i = 0; i < total; i++){
        int curr_deg = 0;
        float change = 0.f;
        int node_id = population[i].id;
        
        int *frontier = (int*)calloc(total, sizeof(int)); //check initialization value
        int *total_visited = (int*)calloc(total, sizeof(int));
        int *total_frontier = (int*)calloc(total*nproc, sizeof(int));
        float *array_change = (float*)calloc(nproc, sizeof(float));

        float total_change = 0.f;

        frontier[i] = 1;
        
        while ((curr_deg != Required_Deg) && (check_frontier(frontier, total))){
            for (int index_inf = 0; index_inf < total; index_inf++){
                // vertex exists in frontier
                total_visited[index_inf] = 1;
                if ((index_inf>=pid*childsize )&& (index_inf<(pid+1)*childsize)){
                    if (frontier[index_inf]){
                        std::vector<Connection> conn = population[index_inf].conn;
                        // check all connections
                        for (Connection person: conn){
                            // add to frontier if connections are not visited
                            if (!total_visited[person.friendID]){
                                change += pow(DR, curr_deg)*person.like*(population[person.friendID].eval);
                                frontier[person.friendID] = 1;
                            } 
                        }
                    }
                }
                frontier[index_inf] = 0;
            }


            //synchronization new frontier and change
            std::cerr << __LINE__ << std::endl;
            for (int j = 0; j < total; j++){
                std::cerr<< frontier[j] << std::endl;
            }
            MPI_Allgatherv(&frontier, total, MPI_INT, &total_frontier, 
            recvcounts, displs, MPI_INT, comm);
            std::cerr << total << std::endl;
            //filter out the repetition in new fronitier and form actual frontier
            for (int fi = 0; fi < total*nproc; fi++){
                if(total_frontier[fi]) frontier[fi%total] = 1;
            }
            MPI_Barrier(MPI_COMM_WORLD);
        }

        // sum up all possible values
        MPI_Allgatherv(&change, 1, MPI_INT, &array_change, 
        recvcounts2, displs2, MPI_INT, comm);
        for(int si = 0; si < nproc; si ++){
            total_change += array_change[si];
            }


        population[i].eval = population[i].eval + change;
        //Synchronize update 

    }
    return population;
}


int main(int argc, char *argv[]) {
    int pid;
    int nproc;
    int total = 0;
    MPI_Status status;
    MPI_Comm comm = MPI_COMM_WORLD;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // Get total number of processes specificed at start of run
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    //StartupOptions options = parseOptions(argc, argv);

    int NforRC = sqrt(nproc);
    int displs[nproc];
    int recvcounts[nproc];
    std::vector<People> population;
    
    //load from input file
    loadFromFile(inputFile, population);

    // int num_rows = sqrt(total);
    // int num_cols = sqrt(total);

    // //rows/cols taken over by each processor
    // int rows_per_proc = num_rows/NforRC;
    // int cols_per_proc = num_rows/NforRC;

    int childsize = total/nproc;

    Timer totalSimulationTimer;
    
    for (int i = 0; i < numIterations; i++){
        population = simulateStep(population, pid, nproc, childsize);
        // MPI_Allgatherv(eval_sample.data(), childsize, MPI_FLOAT, eval_collection.data(), 
        // recvcounts, displs, MPI_FLOAT, comm);


    }

    if(pid == 0){
        //saveToFile(options.outputFile, population);
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("total simulation time: %.6fs\n", totalSimulationTime);
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }

    MPI_Finalize();
}
