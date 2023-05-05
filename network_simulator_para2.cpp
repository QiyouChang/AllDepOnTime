//1D partition BFS
#include "common.h"
#include "timing.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <string>
#include <cmath>
#include <set>

std::string inputFile = "./sample1.txt";
std::string outputFile = "./seqOutputv4.txt";
std::string outputResult = "./seqResultv4.txt";
#define numIterations 5
#define DR 0.9
#define Required_Deg 6

bool check_frontier(int *frontier, int length){
    for (int i = 0; i < length; i++){
        if (frontier[i]) return true;
    }
    return false;
}

std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_collection, int pid, int nproc, int childsize, int* displs, int* recvcounts, int* displs2, int* recvcounts2){
    int total = population.size();
    MPI_Comm comm = MPI_COMM_WORLD;

    for (int i = 0; i < total; i++){
        int curr_deg = 0;
        float change = 0.f;
        int node_id = population[i].id;

        int *frontier = (int*)calloc(total, sizeof(int)); //check initialization value
        int *temp_frontier = (int*)calloc(total, sizeof(int));
        int *total_visited = (int*)calloc(total, sizeof(int));
        int *total_frontier = (int*)calloc(total*nproc, sizeof(int));
        float *array_change = (float*)calloc(nproc, sizeof(float));

        float total_change = 0.f;

        frontier[i] = 1;
        MPI_Barrier(MPI_COMM_WORLD);

        while ((curr_deg != Required_Deg) && (check_frontier(frontier, total))){
            for (int first = 0; first<total ; first++){
                if (frontier[first]){
                    total_visited[first] = 1;
                }
            }

            for (int index_inf = 0; index_inf < total; index_inf++){     

                if ((index_inf>=pid*childsize )&& (index_inf<(pid+1)*childsize)){
                    if(frontier[index_inf]){
                        std::vector<Connection> conn = population[index_inf].conn;
                        // check all connections
                        for (Connection person: conn){
                            // add to frontier if connections are not visited
                            
                            if ((!total_visited[person.friendID])&&(person.like!=0.f)){
                                change += pow(DR, curr_deg)*person.like*(population[person.friendID].eval);
                                temp_frontier[person.friendID] = 1;
                            }
                        }
                    }
                }
                frontier[index_inf] = 0;
            }
            //synchronization new frontier and change

            MPI_Allgatherv(temp_frontier, total, MPI_INT, total_frontier,
            recvcounts, displs, MPI_INT, comm);
            //filter out the repetition in new fronitier and form actual frontier
            for (int fi = 0; fi < total*nproc; fi++){
                if(total_frontier[fi]&&(!total_visited[fi])){
                    frontier[fi%total] = 1;
                } 
                temp_frontier[fi%total] = 0;
            }
            MPI_Barrier(MPI_COMM_WORLD);
            curr_deg += 1;
        }

        // sum up all possible values
        MPI_Allgatherv(&change, 1, MPI_INT, array_change,
        recvcounts2, displs2, MPI_INT, comm);
        for(int si = 0; si < nproc; si ++){
            total_change += array_change[si];
        }
        
        eval_collection[i] = population[i].eval + total_change;
        //Synchronize update

    }

    return eval_collection;
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
    //StartupOptions options = parseOptions(argc, argv);

    int NforRC = sqrt(nproc);
    std::vector<People> population;

    //load from input file
    loadFromFile(inputFile, population);
    int total = population.size();

    std::vector<float> eval_collection;
    eval_collection.resize(population.size());

    int childsize = population.size()/nproc;

    int displs[nproc];
    int recvcounts[nproc];
    std::fill_n(recvcounts, nproc, total);
    for (int i = 0; i < nproc; i++){
        displs[i] = (i == 0) ? 0 : recvcounts[i-1]+displs[i-1];
    }

    int displs2[nproc];
    int recvcounts2[nproc];
    std::fill_n(recvcounts2, nproc, 1);
    for (int i = 0; i < nproc; i++){
        displs2[i] = (i == 0) ? 0 : recvcounts2[i-1]+displs2[i-1];
    }

    Timer totalSimulationTimer;

    for (int i = 0; i < numIterations; i++){
        eval_collection = simulateStep(population, eval_collection, pid, nproc, childsize, displs, recvcounts, displs2, recvcounts2);

        for (int j = 0; j < population.size(); j++){
            population[j].eval = eval_collection[j];
        }
    }

    if(pid == 0){
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("para2 - total simulation time: %.6fs\n", totalSimulationTime);
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }

    MPI_Finalize();
}