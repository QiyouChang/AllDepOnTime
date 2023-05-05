#include "common.h"
#include "timing.h"
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <queue>
#include <omp.h>
#include <atomic>

std::string inputFile = "./sample1.txt"; 
std::string outputFile = "./seqOutputv7.txt"; 
std::string outputResult = "./seqResultv7.txt";
#define numIterations 5
#define DR 0.9
#define Required_Deg 6


float BFS_OneLayer(std::vector<People> &population, std::vector<int> currentFrontier,
         int degree, std::vector<bool> visited, float qEval, int q){
    if(degree == Required_Deg || currentFrontier.size() == 0){
        return qEval;
    }
    
    std::vector<bool> newFrontier(population.size(), false);
    float change = qEval;
    #pragma omp parallel for schedule(auto) num_threads(currentFrontier.size())
    for(int i = 0; i < (int)currentFrontier.size(); i++){
        int currentId = currentFrontier[i];
        
        People person = population[currentId];
        std::vector<Connection> connections = person.conn; 
        
        #pragma omp parallel for schedule(auto) num_threads(connections.size())
        for (size_t j = 0 ; j < (int)connections.size(); j++){
            Connection con = connections[j];
            
            if (!visited[con.friendID] && (con.like!=0.f)){
                newFrontier[con.friendID] = true; 
                visited[con.friendID] = true;
                #pragma omp critical
                {
                    change += pow(DR, degree)*con.like*population[con.friendID].eval;  
                }
            }
        }
       
    }
    //#pragma omp barrier
    std::vector<int> newf;
    //#pragma omp parallel for schedule(auto) num_threads(newFrontier.size())
    for(int i = 0; i < newFrontier.size(); i++){
        if(newFrontier[i]){
            newf.push_back(i);
        }
    }
    return BFS_OneLayer(population, newf, degree+1, visited, change, q);
}

int main(int argc, char *argv[]) {
    std::vector<People> population; 
    
    loadFromFile(inputFile, population); 
    std::vector<float> eval_collection; 
    eval_collection.resize(population.size()); 
    // std::cerr << omp_get_num_threads() << "\n";
    // omp_set_auto(0);
    // omp_set_num_threads(1000);
    // std::cerr << omp_get_num_threads() << "\n";

    //#pragma omp parallel for schedule(auto) num_threads(population.size())
    for (int i = 0; i < (int)eval_collection.size();  i++){
        eval_collection[i] = population[i].eval; 
    }

    Timer totalSimulationTimer; 
    for (int i = 0;i < numIterations; i++) {
        #pragma omp parallel for schedule(auto) num_threads(population.size())
        for(int q = 0; q < (int)population.size(); q++){
            std::vector<int> frontier;
            std::vector<bool> visited(population.size(), false);
            frontier.push_back(q);
            visited[q] = true;
            float value = BFS_OneLayer(population, frontier, 0, visited, eval_collection[q], q);
            eval_collection[q] = value;
        }
        for (int i = 0; i < eval_collection.size();  i++){
            population[i].eval = eval_collection[i]; 
        }
    }
   
    double totalSimulationTime = totalSimulationTimer.elapsed(); 
    printf("total simulation time: %.6fs\n", totalSimulationTime); 
    saveToFile(outputFile, population); 
    saveToResult(outputResult, population); 
}