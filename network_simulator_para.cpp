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
#define Required_Deg 6

float BFS_All(std::set<int> frontier, std::set<int> visited, std::vector<People> &population,std::vector<float> &eval_sample, std::vector<float> &eval_collection, float change, int curr_deg, int pid, std::vector<int> &dg_arr){
    
   if ((frontier.size() == 0) || (curr_deg == Required_Deg)){
        return change;
    }else{
        
        size_t size = frontier.size();

        //pop the top person
        int curr_id = *frontier.begin();
        //erase the person
        frontier.erase(frontier.begin());
        //find the person
        People person = population[curr_id];
        //add to visited 
        visited.insert(curr_id);
        //find out neighbors
        std::vector<Connection> connections = person.conn;

        for (size_t i = 0 ; i < connections.size(); i++){
            if ((visited.find(connections[i].friendID)==visited.end())&& (connections[i].like!=0.f)&&(dg_arr[i]==-1)){
                frontier.insert(connections[i].friendID);
                dg_arr[i] = curr_deg;
                change += pow(DR, curr_deg)*connections[i].like*(eval_collection[connections[i].friendID]);
            }
        }

       return BFS_All(frontier, visited, population, eval_sample, eval_collection,change, curr_deg+1, pid, dg_arr);
    }
}

std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_sample, std::vector<float> &eval_collection, int pid, int childsize){
    int total = population.size();

    for (int i = 0; i < childsize; i ++){
        float change = 0.f;
        int index = i+childsize*pid;
        std::vector<int> dg_arr;
        dg_arr.resize(total);
        for (int j = 0; j < total; j++){
            dg_arr[j] = j==i ? 0 : -1;
        }
        std::set<int> visited = {};
        std::set<int> frontier = {population[index].id};
        change = BFS_All(frontier, visited, population, eval_sample, eval_collection, change, 0, pid, dg_arr);
        eval_sample[i] = eval_collection[index] + change;

    }
    return eval_sample;
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

    int displs[nproc];
    int recvcounts[nproc];

    std::vector<People> population;
    std::vector<float> eval_collection, eval_sample;

    //load from input file
    loadFromFile(inputFile, population);
    total = (int)population.size();

    //calculate child size
    int childsize = total/nproc;

    //initialize recvcounts and displs
    std::fill_n(recvcounts, nproc, childsize);
    for (int i = 0; i < nproc; i++){
        displs[i] = (i == 0) ? 0 : recvcounts[i-1]+displs[i-1];
    }

    //resize collection and sample
    eval_collection.resize(total);
    eval_sample.resize(childsize);
    for (int i = 0; i < total; i++){
        eval_collection[i] = population[i].eval;
    }

    Timer totalSimulationTimer;
    for (int i = 0; i < numIterations; i++){
        eval_sample = simulateStep(population, eval_sample, eval_collection, pid, childsize);
        MPI_Allgatherv(eval_sample.data(), childsize, MPI_FLOAT, eval_collection.data(), 
        recvcounts, displs, MPI_FLOAT, comm);
    }

    if(pid == 0){
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("parar 1 - total simulation time: %.6fs\n", totalSimulationTime);
        for (int i = 0; i < total; i++){
            population[i].eval = eval_collection[i];
        }
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }

    MPI_Finalize();
}