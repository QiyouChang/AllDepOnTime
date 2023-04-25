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
#define numIterations 5
#define DR 0.9
#define Required_Deg 2

bool check_frontier(int *frontier, int length){
    for (int i = 0; i < length; i++){
        if (frontier[i]) return true;
    }
    return false;
}


std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_collection, int pid, int nproc, int childsize){
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
    std::fill_n(recvcounts2, nproc, 1);
    for (int i = 0; i < nproc; i++){
        displs2[i] = (i == 0) ? 0 : recvcounts2[i-1]+displs2[i-1];
    }



    for (int i = 0; i < total; i++){
        int curr_deg = 0;
        float change = 0.f;
        int node_id = population[i].id;

<<<<<<< HEAD
        std::vector<int> dg_arr;
        dg_arr.resize(total);
        for (int j = 0; j < total; j++){
            dg_arr[j] = j==node_id ? 0 : -1 ;
        }

        std::vector<int> total_dg_arr;
        total_dg_arr.resize(total*nproc);
        for (int j = 0; j < total*nproc; j++){
            total_dg_arr[j] = -1;
        }

=======
>>>>>>> 278e315b00d9b9b9d22aed3ecb0165814e3285ac
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
                std::cerr<<"visited: "<<first << " ? "<< frontier[first] << std::endl;
                if (frontier[first]){
<<<<<<< HEAD
                    total_visited[first] = 1;
                }
=======
                total_visited[first] = 1;
            }
>>>>>>> 278e315b00d9b9b9d22aed3ecb0165814e3285ac
            }

            for (int index_inf = 0; index_inf < total; index_inf++){     

                if ((index_inf>=pid*childsize )&& (index_inf<(pid+1)*childsize)){
                        if(frontier[index_inf]){
                        std::vector<Connection> conn = population[index_inf].conn;
                        // check all connections
                        for (Connection person: conn){
                            // add to frontier if connections are not visited
<<<<<<< HEAD
                            
                            if ((!total_visited[person.friendID])&&(person.like!=0.f)&&(dg_arr[person.friendID] == -1)){
                                if(i==0) std::cerr<<"connection person.friendID "<< person.friendID << std::endl;
                                //std::cerr<<"change before: "<< change <<"; pow(DR, curr_deg)"<<pow(DR, curr_deg)<<"; person.like"<<person.like << "; population[person.friendID].eval: "<< population[person.friendID].eval<< std::endl;
                                change += pow(DR, curr_deg)*person.like*(population[person.friendID].eval);
                                temp_frontier[person.friendID] = 1;
                                dg_arr[person.friendID] = curr_deg;
                                
=======
                            if ((!total_visited[person.friendID])&&(person.like!=0.f)){
                                //std::cerr<<"change before: "<< change <<"; pow(DR, curr_deg)"<<pow(DR, curr_deg)<<"; person.like"<<person.like << "; population[person.friendID].eval: "<< population[person.friendID].eval<< std::endl;
                                change += pow(DR, curr_deg)*person.like*(population[person.friendID].eval);
                                temp_frontier[person.friendID] = 1;
                                if(i==0) std::cerr<<"connection person.friendID "<< person.friendID << std::endl;
>>>>>>> 278e315b00d9b9b9d22aed3ecb0165814e3285ac
                            }
                        }
                        }
                    }
<<<<<<< HEAD
                frontier[index_inf] = 0;
                MPI_Allgatherv(dg_arr.data(), total, MPI_INT, total_dg_arr.data(),
                recvcounts, displs, MPI_INT, comm);
                 for (int fi = 0; fi < total*nproc; fi++){
                    dg_arr[fi%total] = ((total_dg_arr[fi] != -1) && (dg_arr[fi%total] == -1)) ? total_dg_arr[fi] : std::min(total_dg_arr[fi], dg_arr[fi%total]);
                }
            }
=======
                    frontier[index_inf] = 0;
                }
>>>>>>> 278e315b00d9b9b9d22aed3ecb0165814e3285ac
                

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
        std::cerr<< "total change: "<< total_change<<std::endl;
        eval_collection[i] = population[i].eval + total_change;
        //Synchronize update

    }


    return eval_collection;
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

    std::vector<float> eval_collection;
    eval_collection.resize(population.size());

    // int num_rows = sqrt(total);
    // int num_cols = sqrt(total);

    // //rows/cols taken over by each processor
    // int rows_per_proc = num_rows/NforRC;
    // int cols_per_proc = num_rows/NforRC;

    int childsize = population.size()/nproc;

    Timer totalSimulationTimer;

    for (int i = 0; i < numIterations; i++){
        eval_collection = simulateStep(population, eval_collection, pid, nproc, childsize);
        // MPI_Allgatherv(eval_sample.data(), childsize, MPI_FLOAT, eval_collection.data(),
        // recvcounts, displs, MPI_FLOAT, comm);
    //     for(int m = 0; m< total; m++){
    //     std::cerr<<eval_collection[m]<< std::endl;
    // }

        for (int j = 0; j < population.size(); j++){
            population[j].eval = eval_collection[j];
        }

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
