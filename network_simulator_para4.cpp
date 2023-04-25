#include "common.h"
#include "timing.h"
#include <iostream>
#include "mpi.h"
#include <vector>
#include <string>
#include <cmath>
#include <set>
#include <queue>

std::string inputFile = "./sample1.txt";
std::string outputFile = "./seqOutputv4.txt";
std::string outputResult = "./seqResultv4.txt";
#define numIterations 10
#define DR 0.9
#define Required_Deg 2

float BFS_All(std::set<int> frontier, std::set<int> visited, std::vector<People> &population,std::vector<float> &eval_sample, std::vector<float> &eval_collection, float change, int curr_deg, int pid){
    
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
                        change += pow(DR, curr_deg)*connections[i].like*(eval_collection[connections[i].friendID]);
                        //change += connections[i].like;
                    }
                }

       return BFS_All(frontier, visited, population, eval_sample, eval_collection,change, curr_deg+1, pid);
    }
}

bool peopleWorkComparison(const People &a, const People &b){
    return a.conn.size() < b.conn.size();
}


struct procPeople{
    int procID;
    int size;
    std::vector<People> peoples;
};

class Compare{
public:
    bool operator()(const procPeople &a, const procPeople &b){
        return a.procID < b.procID;
    }
};


template<typename T>
std::vector<T> flatten(std::vector<std::vector<T>> const &vec)
{
    std::vector<T> flattened;
    for (auto const &v: vec) {
        flattened.insert(flattened.end(), v.begin(), v.end());
    }
    return flattened;
}


// Input: population: the list of people read from file 
// Input: number of processors 
// Output: recvCounts: the recvCounts used to distribute people to processors later 
// Output: newPopulation: list of people where people assigned to same processor is next to each other
void prediction(std::vector<People> &population,int nproc, std::vector<int> &recvCounts, std::vector<People> &newPopulation){
    int workEstimate[population.size()];//work estimate of 2d separation for a person
    int numFriend[population.size()];  //work estimate of a single person 1d separation
    //run the 1d work estimation
    for(int i = 0; i < population.size(); i++){
        People p = population[i];
        numFriend[i] = p.conn.size();
        workEstimate[i] = p.conn.size();
    }
    //also see how much work friends of a person has to predict the overall work a person's BFS might require
    for(int j = 0; j < population.size(); j++){
        People p = population[j];
        std::vector<Connection> cn = p.conn;
        for(int k = 0; k < cn.size(); k++){
            workEstimate[j] += numFriend[cn[k].friendID];
        }
    }

    //sort the population based on work
    std::sort(population.begin(), population.end(), &peopleWorkComparison);
    
    std::priority_queue<procPeople, std::vector<procPeople>, Compare> priorityQueue;
    for(int i = 0; i < nproc; i++){
        People p = population[i];
        std::vector<People> currentProcPeople = {p};
        procPeople currentProc = {i, p.conn.size(), currentProcPeople};
        priorityQueue.push(currentProc);
    }

    int count = nproc;
    while(count < population.size()){
        procPeople q = priorityQueue.top();
        priorityQueue.pop();
        People currentPeople = population[count];
        q.size += numFriend[currentPeople.id];
        q.peoples.push_back(currentPeople);
        priorityQueue.push(q);
        count += 1;
    }
    
    std::vector<std::vector<People>> peopleAssignment(nproc);
    
    for(int c = 0; c<nproc; c++){
        procPeople q = priorityQueue.top();
        priorityQueue.pop();
        peopleAssignment[q.procID] = q.peoples; 
        recvCounts[q.procID] = q.peoples.size();
    }

    newPopulation = flatten(peopleAssignment);
}


std::vector<float> simulateStep(std::vector<People> &population, std::vector<float> &eval_sample, std::vector<float> &eval_collection, int pid, int childsize){
    int totla = population.size();

    for (int i = 0; i < childsize; i ++){
        float change = 0.f;
        
        int index = i+childsize*pid;
        std::set<int> visited = {};
        std::set<int> frontier = {population[index].id};

        change = BFS_All(frontier, visited, population, eval_sample, eval_collection, change, 0, pid);

        eval_sample[i] = eval_collection[index] + change;
        //Synchronize update 

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
        // for (int j = 0; j < total; j++){
        //     printf("%f\n", eval_collection[j]);
        // }

    }

    if(pid == 0){
        //saveToFile(options.outputFile, population);
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("total simulation time: %.6fs\n", totalSimulationTime);
        for (int i = 0; i < total; i++){
            population[i].eval = eval_collection[i];
        }
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }

    MPI_Finalize();
}