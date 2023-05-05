#include "common.h"
#include "timing.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <queue>
#include <omp.h>


std::string inputFile = "./sample1.txt"; 
std::string outputFile = "./seqOutputv6.txt"; 
std::string outputResult = "./seqResultv6.txt";
#define numIterations 5
#define DR 0.9
#define Required_Deg 6
#define nproc 4

std::vector<int> dg_arr;
int curr_deg = 0;
std::set<int> frontier;
std::set<int> visited; 
float change = 0.0; 

void BFS_OneLayer(std::vector<People> &population, std::set<int> currentFrontier, int degree){
    std::set<int> newFrontier;
    #pragma omp parallel for
    for(int i = 0; i < currentFrontier.size(); i++){
        #pragma omp critical
        {
            int curr_id = *currentFrontier.begin();
            currentFrontier.erase(currentFrontier.begin());
            People person = population[curr_id];
        }
    }
}


void BFS_All(std::vector<People> &population){
    while ((frontier.size() != 0) && (curr_deg != Required_Deg)){
        
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

        #pragma omp parallel for
        for (size_t i = 0 ; i < connections.size();  i++){
            if ((visited.find(connections[i].friendID)==visited.end())&& (connections[i].like!=0.f)&&(dg_arr[i]==-1)){
                #pragma omp critical
                {
                    dg_arr[i] = curr_deg; 
                    frontier.insert(connections[i].friendID); 
                }
                change += pow(DR, curr_deg)*connections[i].like*(population[connections[i].friendID].eval); 
            }
        }
        //#pragma omp barrier
        
        curr_deg += 1; 
    }
}

std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_collection){
    // Sequential Implementation that simply computes all weights of people 
    // and try adding them together
    int total = population.size();
    
    #pragma omp parallel for
    for(int i = 0; i < total;  i ++){
        change = 0.f; 
        dg_arr.resize(total); 
        for (int j = 0;  j < total; j++){
            dg_arr[j] = j==i ? 0 : -1; 
        }
        People person = population[i]; 
        visited = {}; 
        frontier = {person.id}; 
        curr_deg = 0; 
        BFS_All(population); 
        eval_collection[i] = population[i].eval + change; 
        //Asynchronize Update
    }
    return eval_collection; 
}

int main(int argc, char *argv[]) {
   
    std::vector<People> population; 
    
    loadFromFile(inputFile, population); 
    std::vector<float> eval_collection; 
    eval_collection.resize(population.size()); 
    //omp_set_num_threads(10); 
    for (int i = 0; i < eval_collection.size();  i++){
        eval_collection[i] = population[i].eval; 
    }

    Timer totalSimulationTimer; 
    for (int i = 0;i < numIterations; i++) {
       
        eval_collection = simulateStep(population, eval_collection); 
        for (int i = 0; i < eval_collection.size();  i++){
            population[i].eval = eval_collection[i]; 
        }
    }

    double totalSimulationTime = totalSimulationTimer.elapsed(); 
    printf("total simulation time: %.6fs\n", totalSimulationTime); 
    saveToFile(outputFile, population); 
    saveToResult(outputResult, population); 
}