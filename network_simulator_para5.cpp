#include <iostream>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <tbb/mutex.h>
#include <cilk/reducer_opadd.h>

#define DR 0.9
#define Required_Deg 2
#define GRAINULARITY 10
// Multiset attempt
// struct Node{
//     People p;
//     int treeSize;
//     Node *left;
//     Node *right;
// }

// int bagSize(std::multiset<Node> bag){
//     int size = 0;
//     for(std::multiset<Node>::iterator itr = bag.begin(); itr != bag.end(); itr++){
//         size += *itr.treeSize;
//     }
//     return size;
// }

// std::multiset<Node> bagCreate(){
//     std::multiset<Node> bag;
//     return bag;
// }

// void bagInsert(std::multiset<Node> bag, Node v){

// }

int bagSize(std::set<People> bag){
    return (int)bag.size();
}

void bagInsert(std::set<People> bag, People p){
    bag.insert(p);
    return;
}


std::set<People> bagSplit(std::set<People> bag){
    std::set<People> new_bag1;
    std::set<People> new_bag2;
    int size = bagSize(bag);
    int count = 0;
    for(auto it = bag.begin(); it != bag.end(); it++){
        if(count < size/2){
            new_bag1.insert(*it);
            count += 1;
        } else{
            new_bag2.insert(*it);
        }
       
    }
    bag = new_bag2;
    return new_bag1;
}

int processLayer(std::set<People> in_bag, std::set<People> out_bag, std::vector<People> population, int depth, People startp){
    int *total_visited = (int*)calloc(population.size(), sizeof(int));
    std::mutex my_mutex;
    cilk_reduce_opadd<int> change;
    if(depth == 0){
        return 0;
    }
    if(bagSize(in_bag) < GRAINULARITY){
        for(People currentp: in_bag){
            Connection pconn = currentp.conn;
            cilk_for(int i = 0; i < pconn.size(); i++){
                int neighorID = pconn[i].friendID;
                if(total_visited[neighborID] == 0){
                    my_mutex.lock();
                    total_visited[neighborID] = 1;
                    bagInsert(out_bag, population[friendID]);
                    my_mutex.unlock();
                    change += pow(DR, depth) * pconn[i].like * total_visited[neighborID].eval;
                }
            }
        }
        return 0;
    }
    new_bag = bagSplit(in_bag);
    cilk_spawn processLayer(new_bag, out_bag, population, depth);
    processLayer(new_bag, out_bag, population, depth);
    cilk_sync;
    return change;
}

int main(int argc, char *argv[]) {
    int pid;
    int nproc;


    std::vector<People> population;

    //load from input file
    loadFromFile(inputFile, population);
    int total = population.size();

    Timer totalSimulationTimer;

    for (int i = 0; i < numIterations; i++){
        for (int j = 0; j < population.size(); j++){
            People currentp = population[j];
            std::set<People> in_bag;
            std::set<People> out_bag;
            for(int k = 0; k < currentp.conn.size(); k++){
                int neighborID = currentp.conn[k];
                in_bag.insert(population[neighborID]);
            }
            population[j].eval = processLayer(in_bag, out_bag, population, Required_Deg, population[i]);
        }
    }

    if(pid == 0){
        //saveToFile(options.outputFile, population);
        double totalSimulationTime = totalSimulationTimer.elapsed();
        printf("total simulation time: %.6fs\n", totalSimulationTime);
        saveToFile(outputFile, population);
        saveToResult(outputResult, population);
    }
}





