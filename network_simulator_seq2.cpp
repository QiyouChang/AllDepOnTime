#include "common.h"
#include "timing.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <string>
#include <cmath>
#include <set>

std::string inputFile = "./sample1.txt";
std::string outputFile = "./seqOutputv2.txt";
std::string outputResult = "./seqResultv2.txt";
#define numIterations 10
#define DR 0.9
#define Required_Deg 2


float BFS_All(std::set<int> frontier, std::set<int> visited, std::vector<People> &population, float change, int curr_deg){
    if ((frontier.size() == 0) || (curr_deg == Required_Deg)){
        //std::cerr<< "the change is: " << change<< "\n";
        return change;
    }else{
        
        //std::cerr<< "Ever reached here?";
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
                if ((visited.find(connections[i].friendID)==visited.end())&& (connections[i].like!=0.f)){
                        frontier.insert(connections[i].friendID);
                        change += pow(DR, curr_deg)*connections[i].like*(population[connections[i].friendID].eval);
                        //change += connections[i].like;
                    }
                }
        return BFS_All(frontier, visited, population, change, curr_deg+1);

    //     for (int turn = 0; turn < size; turn++){

    //         int curr_id = *frontier.begin();
    //         //std::cerr<< curr_id << "id \n";
    //         frontier.erase(frontier.begin());
    //         People person = population[curr_id];
    //         std::vector<Connection> connections = person.conn;
    //         //not visited yet
    //         if (visited.find(curr_id)==visited.end()){
    //             for (size_t i = 0 ; i < connections.size(); i++){
    //                 //not visited yet and still connected
    //                 if ((visited.find(connections[i].friendID)==visited.end())&& (abs(connections[i].like)>0.0001f)){
                        
    //                     new_frontier.insert(connections[i].friendID);
    //                     change += pow(DR, curr_deg)*connections[i].like*(population[connections[i].friendID].eval);
    //                 }
    //             }
    //         }
    //         visited.insert(curr_id);
    //     }
    //    return BFS_All(new_frontier, visited, population, change, curr_deg+1);
    }
}

std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_collection){
    // Sequential Implementation that simply computes all weights of people 
    // and try adding them together
    int total = population.size();
    for(int i = 0; i < total; i ++){
        float change = 0.f;
        People person = population[i];
        std::set<int> visited = {};
        std::set<int> frontier = {person.id};
        change = BFS_All(frontier, visited, population, change, 0);
        //printf("%f, %f, %f, %f\n",change, eval_collection[i], population[i].conn[0].like, population[i].conn[1].like);
        eval_collection[i] = population[i].eval + change;
        //Asynchronize Update
    }
    return eval_collection;
}

int main(int argc, char *argv[]) {
    int pid;
    int nproc;
    //MPI_Status status;
    //MPI_Comm comm = MPI_COMM_WORLD;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    // Get process rank
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);
    // Get total number of processes specificed at start of run
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    //StartupOptions options = parseOptions(argc, argv);
    std::vector<People> population;
    if (pid == 0) {
        //loadFromFile(options.inputFile, population);
        loadFromFile(inputFile, population);
    }

    std::vector<float> eval_collection;
    eval_collection.resize(population.size());
    for (int i = 0; i < eval_collection.size(); i++){
        eval_collection[i] = population[i].eval;
        //printf("%f, %f, %f\n", eval_collection[i], population[i].conn[0].like, population[i].conn[1].like);
    }


    Timer totalSimulationTimer;
    for (int i = 0; i < numIterations; i++) {
        eval_collection = simulateStep(population, eval_collection);
        for (int i = 0; i < eval_collection.size(); i++){
            population[i].eval = eval_collection[i];
        }
        // for (int j = 0; j < population.size(); j++){
        //     printf("%f\n", eval_collection[j]);
        // }
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