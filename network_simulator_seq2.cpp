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
#define numIterations 5
#define DR 0.9
#define Required_Deg 6


float BFS_All(std::set<int> frontier, std::set<int> visited, std::vector<People> &population, float change, int curr_deg, std::vector<int> &dg_arr){
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
                dg_arr[i] = curr_deg;
                frontier.insert(connections[i].friendID);
                change += pow(DR, curr_deg)*connections[i].like*(population[connections[i].friendID].eval);
            }
        }
        return BFS_All(frontier, visited, population, change, curr_deg+1, dg_arr);
    }
}

std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_collection){
    // Sequential Implementation that simply computes all weights of people 
    // and try adding them together
    int total = population.size();
    for(int i = 0; i < total; i ++){
        float change = 0.f;
        std::vector<int> dg_arr;
        dg_arr.resize(total);
        for (int j = 0; j < total; j++){
            dg_arr[j] = j==i ? 0 : -1;
        }
        People person = population[i];
        std::set<int> visited = {};
        std::set<int> frontier = {person.id};
        change = BFS_All(frontier, visited, population, change, 0, dg_arr);
        eval_collection[i] = population[i].eval + change;
        //Asynchronize Update
    }
    return eval_collection;
}

int main(int argc, char *argv[]) {

    std::vector<People> population;
    if (pid == 0) {
        loadFromFile(inputFile, population);
    }

    std::vector<float> eval_collection;
    eval_collection.resize(population.size());
    for (int i = 0; i < eval_collection.size(); i++){
        eval_collection[i] = population[i].eval;
    }

    Timer totalSimulationTimer;
    for (int i = 0; i < numIterations; i++) {
        eval_collection = simulateStep(population, eval_collection);
        for (int i = 0; i < eval_collection.size(); i++){
            population[i].eval = eval_collection[i];
        }
    }

    if(pid == 0){
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("seq - total simulation time: %.6fs\n", totalSimulationTime);
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }
    MPI_Finalize();
}