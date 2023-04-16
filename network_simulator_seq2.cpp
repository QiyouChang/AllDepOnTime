
#include "common.h"
#include "timing.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <string>
#include <cmath>
#include <set>

std::string inputFile = "./benchmark_files/sample1.txt";
std::string outputFile = "./seqOutputv2.txt";
std::string outputResult = "./seqResultv2.txt";
#define numIterations 5
#define DR 0.9
#define Required_Deg 2

// float recursive_eval (int j, std::vector<People> &population, std::set<int> idset, float change, int curr_deg){
//     if (curr_deg == Required_Deg){
//         change += population[j].eval*pow(DR,curr_deg);
//     }else{
//         People person = population[j];
//         change += person.eval*pow(DR,curr_deg);
//         std::vector<Connection> connections = person.conn;
//         for (size_t i = 0 ; i < connections.size(); j++){
//             change = recursive_eval(i, population, idset, change, curr_deg+1);
//         }
//     }
//     return change;
// }

float BFS_All(std::set<int> frontier, std::set<int> visited, std::vector<People> &population, float change, int curr_deg){
    if ((frontier.size() == 0) || (curr_deg == Required_Deg)){
        //std::cerr<< "the change is: " << change<< "\n";
        return change;
    }else{
        //std::cerr<< "Ever reached here?";
        std::set<int> new_frontier = {};
        size_t size = frontier.size();
        for (int turn = 0; turn < size; turn++){
            int curr_id = *frontier.begin();
            //std::cerr<< curr_id << "id \n";
            frontier.erase(frontier.begin());
            People person = population[curr_id];
            std::vector<Connection> connections = person.conn;
            //not visited yet
            if (visited.find(curr_id)==visited.end()){
                for (size_t i = 0 ; i < connections.size(); i++){
                    //not visited yet and still connected
                    if ((visited.find(connections[i].friendID)==visited.end())&& (abs(connections[i].like)>0.0001f)){
                        
                        new_frontier.insert(connections[i].friendID);
                        change += pow(DR, curr_deg)*connections[i].like;
                    }
                }
            }
            visited.insert(curr_id);
        }
       return BFS_All(new_frontier, visited, population, change, curr_deg+1);
    }
}

std::vector<People> simulateStep(std::vector<People> &population){
    // Sequential Implementation that simply computes all weights of people 
    // and try adding them together
    int total = population.size();
    for(int i = 0; i < total; i ++){
        float change = 0.f;
        People person = population[i];
        std::set<int> visited = {};
        std::set<int> frontier = {person.id};
        change = BFS_All(frontier, visited, population, change, 0);
        population[i].eval += change;
    }
    return population;
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
    //TO DO: fill in the steps
    for (int i = 0; i < numIterations; i++) {
        population = simulateStep(population);
        randomizedPeople(population, 1);
    }

    if(pid == 0){
        //saveToFile(options.outputFile, population);
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }
    MPI_Finalize();
}