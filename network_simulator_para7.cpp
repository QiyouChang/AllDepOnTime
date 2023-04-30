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
    // std::cerr << "the size of visited is: " << visited.size() << "the size of currentFrontier is: " 
    //     << currentFrontier.size() << "the current degree is: " << degree << "for node" << q << "\n";
    if(degree == Required_Deg || currentFrontier.size() == 0){
        return qEval;
    }
    
    std::vector<bool> newFrontier(population.size(), false);
    float change = qEval;
    #pragma omp parallel for schedule(dynamic) num_threads(currentFrontier.size())
    for(int i = 0; i < (int)currentFrontier.size(); i++){
        int currentId = currentFrontier[i];
        
        People person = population[currentId];
        std::vector<Connection> connections = person.conn; 
        
        //#pragma omp parallel for schedule(dynamic) num_threads(connections.size())
        for (size_t j = 0 ; j < (int)connections.size(); j++){
            Connection con = connections[j];
            
            if (!visited[con.friendID] && (con.like!=0.f)){
                //std::cerr << "finish get connection" << con.friendID << "change is: " << change << "\n";
                //std::cerr << "like is " << con.like << "power p is: " << pow(DR, degree) << "eval: " << population[con.friendID].eval << "\n";
                newFrontier[con.friendID] = true; 
                visited[con.friendID] = true;
                // #pragma omp critical
                // {
                    change += pow(DR, degree)*con.like*population[con.friendID].eval;  
                //}
            }
        }
       
    }
    //#pragma omp barrier
    std::vector<int> newf;
    #pragma omp parallel for schedule(dynamic) num_threads(newFrontier.size())
    for(int i = 0; i < newFrontier.size(); i++){
        if(newFrontier[i]){
            newf.push_back(i);
        }
    }
    //std::cerr << "The number of new frontier is: " << newf.size() << "the node is:" << q << " the current degree is: " << degree <<  "change is:" <<  change << "\n";
    return BFS_OneLayer(population, newf, degree+1, visited, change, q);
}

int main(int argc, char *argv[]) {
    std::vector<People> population; //std::cerr<< __LINE__<<std::endl;
    
    loadFromFile(inputFile, population); //std::cerr<< __LINE__<<std::endl;
    std::vector<float> eval_collection; //std::cerr<< __LINE__<<std::endl;
    eval_collection.resize(population.size()); //std::cerr<< __LINE__<<std::endl;
    // std::cerr << omp_get_num_threads() << "\n";
    // omp_set_dynamic(0);
    // omp_set_num_threads(1000);
    // std::cerr << omp_get_num_threads() << "\n";
    //#pragma omp parallel for schedule(dynamic) num_threads(population.size())
    for (int i = 0; i < (int)eval_collection.size();  i++){
        eval_collection[i] = population[i].eval; 
        //std::cerr<< "read here: " << population[i].eval<<std::endl;
    }

    Timer totalSimulationTimer; //std::cerr<< __LINE__<<std::endl;
    for (int i = 0;i < numIterations; i++) {
        //std::cerr << "is here! here is: iteration " << i << "\n"; 
        #pragma omp parallel for schedule(dynamic) num_threads(population.size())
        for(int q = 0; q < (int)population.size(); q++){
            std::vector<int> frontier;
            std::vector<bool> visited(population.size(), false);
            frontier.push_back(q);
            visited[q] = true;
            //std::cerr << "In iteration " << i << " for node" << q << "size of visited is"  << visited.size()<<"\n"; 
            float value = BFS_OneLayer(population, frontier, 0, visited, eval_collection[q], q);
            //float value = 0.0;
            //std::cerr << "in current iteration, we previously have: " << eval_collection[q] << "we now have:" << value << "\n"; 
            eval_collection[q] = value;
        }
        for (int i = 0; i < eval_collection.size();  i++){
        //std::cerr<<"we start with: " << population[i].eval << "end with " << eval_collection[i] << "\n";
            population[i].eval = eval_collection[i]; //std::cerr<< __LINE__<<std::endl;
        }
    }
   
    double totalSimulationTime = totalSimulationTimer.elapsed(); //std::cerr<< __LINE__<<std::endl;
    printf("total simulation time: %.6fs\n", totalSimulationTime); //std::cerr<< __LINE__<<std::endl;
    saveToFile(outputFile, population); //std::cerr<< __LINE__<<std::endl;
    saveToResult(outputResult, population); //std::cerr<< __LINE__<<std::endl;
}