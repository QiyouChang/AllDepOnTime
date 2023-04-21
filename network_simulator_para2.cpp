
// #include "common.h"
// #include "timing.h"
// #include <iostream>
// #include "mpi.h"
// #include <vector>
// #include <string>
// #include <cmath>
// #include <set>

// std::string inputFile = "./benchmark_files/sample1.txt";
// std::string outputFile = "./seqOutputv2.txt";
// std::string outputResult = "./seqResultv2.txt";
// #define numIterations 5
// #define DR 0.9
// #define Required_Deg 2

// float BFS_All(std::set<int> frontier, std::set<int> visited, std::vector<People> &population, float change, int curr_deg){
//     if ((frontier.size() == 0) || (curr_deg == Required_Deg)){
//         //std::cerr<< "the change is: " << change<< "\n";
//         return change;
//     }else{
//         //std::cerr<< "Ever reached here?";
//         std::set<int> new_frontier = {};
//         size_t size = frontier.size();
//         for (int turn = 0; turn < size; turn++){
//             int curr_id = *frontier.begin();
//             //std::cerr<< curr_id << "id \n";
//             frontier.erase(frontier.begin());
//             People person = population[curr_id];
//             std::vector<Connection> connections = person.conn;
//             //not visited yet
//             if (visited.find(curr_id)==visited.end()){
//                 for (size_t i = 0 ; i < connections.size(); i++){
//                     //not visited yet and still connected
//                     if ((visited.find(connections[i].friendID)==visited.end())&& (abs(connections[i].like)>0.0001f)){
                        
//                         new_frontier.insert(connections[i].friendID);
//                         change += pow(DR, curr_deg)*connections[i].like;
//                     }
//                 }
//             }
//             visited.insert(curr_id);
//         }
//        return BFS_All(new_frontier, visited, population, change, curr_deg+1);
//     }
// }

// std::vector<People> simulateStep(std::vector<People> &population, std::vector<People> &sample_population, int pid, int chunksize){
//     // Sequential Implementation that simply computes all weights of people 
//     // and try adding them together
//     int total = population.size();
//     for(int i = 0; i < chunksize; i ++){
//         float change = 0.f;
//         int index = i+chunck_size*pid;
//         People person = population[index];
//         std::set<int> visited = {};
//         std::set<int> frontier = {person.id};
//         change = BFS_All(frontier, visited, population, change, 0);
//         sample_population[i] = population[index].eval + change;
//     }
//     return sample_population;
// }

// int main(int argc, char *argv[]) {
//     int pid;
//     int nproc;
//     size_t size = 0;
//     int Psize = sizeof(Particle);
//     //MPI_Status status;
//     //MPI_Comm comm = MPI_COMM_WORLD;

//     // Initialize MPI
//     MPI_Init(&argc, &argv);
//     // Get process rank
//     MPI_Comm_rank(MPI_COMM_WORLD, &pid);
//     // Get total number of processes specificed at start of run
//     MPI_Comm_size(MPI_COMM_WORLD, &nproc);
//     //StartupOptions options = parseOptions(argc, argv);


//     int displs[nproc];
//     int recvcounts[nproc];

//     std::vector<People> population, sample_population;
//     if (pid == 0) {
//         //loadFromFile(options.inputFile, population);
//         loadFromFile(inputFile, population);
//         size = (int)population.size();
//     }

//     //broadcast population to different processes
//     MPI_Bcast(&size, 1, MPI_INT, 0, comm);

//     int chunksize = size/nproc;

//     std::fill_n(recvcounts, nproc, (chunksize)*Psize);
//     recvcounts[nproc-1] = (size-(nproc-1)*chunksize)*Psize;
//     for (int i = 0; i < nproc; i++){
//         displs[i] = (i == 0) ? 0 : recvcounts[i-1]+displs[i-1];
//     }

//     population.resize(size);
//     sample_population.resize(chunksize);
//     MPI_Bcast(population.data(), sizeof(People) * size, MPI_BYTE, 0, MPI_COMM_WORLD);

//     Timer totalSimulationTimer;
//     //TO DO: fill in the steps
//     for (int i = 0; i < numIterations; i++) {
//         sample_population = simulateStep(population, sample_population, pid, chunksize);
//         MPI_Allgatherv(sample_population.data(), chunksize*Psize, MPI_BYTE, population.data(), 
//         recvcounts, displs, MPI_BYTE, comm);
//         //randomizedPeople(population, 1); randomize the data will influence the intermediate computation
//     }

//     if(pid == 0){
//         //saveToFile(options.outputFile, population);
//         double totalSimulationTime = totalSimulationTimer.elapsed();
//         printf("total simulation time: %.6fs\n", totalSimulationTime);
//         saveToFile(outputFile, population);
//         saveToResult(outputResult, population);
//     }
//     MPI_Finalize();
// }