// //#include <iostream>
// #include "cilk/cilk.h"
// #include "cilk/cilk_api.h"
// #include <mutex>
// #include "cilk/reducer_opadd.h"
// #include <set>
// #include "common.h"
// #include "timing.h"
// using namespace std;
// std::string inputFile = "./sample1.txt";
// std::string outputFile = "./seqOutputv5.txt";
// std::string outputResult = "./seqResultv5.txt";
// #define DR 0.9
// #define Required_Deg 2
// #define numIterations 5
// #define GRAINULARITY 10
// // Multiset attempt
// // struct Node{
// //     People p;
// //     int treeSize;
// //     Node *left;
// //     Node *right;
// // }

// // int bagSize(std::multiset<Node> bag){
// //     int size = 0;
// //     for(std::multiset<Node>::iterator itr = bag.begin(); itr != bag.end(); itr++){
// //         size += *itr.treeSize;
// //     }
// //     return size;
// // }

// // std::multiset<Node> bagCreate(){
// //     std::multiset<Node> bag;
// //     return bag;
// // }

// // void bagInsert(std::multiset<Node> bag, Node v){

// // }

// int bagSize(std::set<People> bag){
//     return (int)bag.size();
// }

// bool bagInsert(std::set<People> bag, People p){
//     int beforeSize = (int)bag.size();
//     bag.insert(p);
//     int afterSize = bagSize(bag);
//     return beforeSize == afterSize;
// }


// std::set<People> bagSplit(std::set<People> bag){
//     std::set<People> new_bag1;
//     std::set<People> new_bag2;
//     int size = bagSize(bag);
//     int count = 0;
//     for(auto it = bag.begin(); it != bag.end(); it++){
//         if(count < size/2){
//             new_bag1.insert(*it);
//             count += 1;
//         } else{
//             new_bag2.insert(*it);
//         }
       
//     }
//     bag = new_bag2;
//     return new_bag1;
// }

// float processLayer(std::set<People> in_bag, std::set<People> out_bag, std::vector<People> population, 
//                 int depth, People startp, float pastChange){
//     int total_visited[population.size()];
//     std::mutex my_mutex;
//     cilk::reducer_opadd<float> change;
//     change.set_value(pastChange);
//     if(depth == 0){
//         return change.get_value();
//     }
//     if(bagSize(in_bag) < GRAINULARITY){
//         for(People currentp: in_bag){
//             std::vector<Connection> pconn = currentp.conn;
//             cilk_for (int i = 0; i < pconn.size(); i++){
//                 int neighorID = pconn[i].friendID;
//                 if(total_visited[neighborID] == 0){
//                     my_mutex.lock();
//                     total_visited[neighborID] = 1;
//                     bool seenBefore = bagInsert(out_bag, population[friendID]);
                    
//                     my_mutex.unlock();
//                     if(!seenBefore){
//                         change += pow(DR, depth) * pconn[i].like * total_visited[neighborID].eval;
//                     }
//                 }
//             }
//         }
//         return 0;
//     }
//     std::set<People> new_bag = bagSplit(in_bag);
//     cilk_spawn processLayer(new_bag, out_bag, population, depth, startp, pastChange);
//     processLayer(new_bag, out_bag, population, depth, startp, pastChange);
//     cilk_sync;
//     std::set<People> nextOut;
//     float val = processLayer(out_bag, nextOut, population, depth-1, startp, change.get_value());
//     return val;
// }

// int main(int argc, char *argv[]) {
//     int pid;
//     int nproc;

//     std::vector<People> population;

//     //load from input file
//     loadFromFile(inputFile, population);
//     int total = population.size();

//     Timer totalSimulationTimer;

//     for (int i = 0; i < numIterations; i++){
//         cilk_for (int j = 0; j < population.size(); j++){
//             People currentp = population[j];
//             std::set<People> in_bag;
//             std::set<People> out_bag;
//             cilk_for(int k = 0; k < currentp.conn.size(); k++){
//                 int neighborID = currentp.conn[k];
//                 in_bag.insert(population[neighborID]);
//             }
//             population[j].eval = processLayer(in_bag, out_bag, population, Required_Deg, population[j]);
//         }
//     }

//     if(pid == 0){
//         //saveToFile(options.outputFile, population);
//         double totalSimulationTime = totalSimulationTimer.elapsed();
//         printf("total simulation time: %.6fs\n", totalSimulationTime);
//         saveToFile(outputFile, population);
//         saveToResult(outputResult, population);
//     }
// }





