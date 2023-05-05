#ifndef COMMON_H_
#define COMMON_H_

#include <cassert>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <numeric>
#include <algorithm>

struct StartupOptions {
  int numIterations = 5;
  int numPeople = 500;
  std::string outputFile;
  std::string inputFile;
};

inline StartupOptions parseOptions(int argc, char *argv[]) {
  StartupOptions rs;
  for (int i = 1; i < argc; i++) {
    if (i < argc - 1) {
      if (strcmp(argv[i], "-i") == 0)
        rs.numIterations = atoi(argv[i + 1]);
      else if (strcmp(argv[i], "-in") == 0)
        rs.inputFile = argv[i + 1];
      else if (strcmp(argv[i], "-n") == 0)
        rs.numPeople = atoi(argv[i + 1]);
      else if (strcmp(argv[i], "-o") == 0)
        rs.outputFile = argv[i + 1];
    }
  }
  return rs;
}

struct Connection{
    int friendID;
    float like;
};

struct People {
    int id;
    float eval; 
    std::vector<Connection> conn;
};

bool compareConn(Connection a, Connection b){
  return (abs(a.like) < abs(b.like));
}

float findAverage(std::vector<Connection> c){
  float count = 0;
  for(size_t x = 0; x < c.size(); x++){
    count += c[x].like;
  }
  return count / (int)c.size();
}

inline void randomizedPeople(std::vector<People> &population, int deleteNum){
  std::map<int, std::vector<Connection>> addConnection; 
  std::map<int, std::vector<Connection>> reduceConnection;
  //prepopulate the map 

  for(size_t i = 0; i < population.size(); i++){
    std::vector<Connection> newAdd;
    std::vector<Connection> newReduce;
    addConnection[i] = newAdd;
    reduceConnection[i] = newReduce;
    People person = population[i];
    std::vector<Connection> conn = person.conn;
    std::sort(conn.begin(), conn.end(), compareConn);
    Connection newConnection;
    
    //add a connection by establishing a connection from like of 0 to average of all connections.
    if(abs(conn[1].like) < 0.001f){
      newConnection.like = findAverage(conn);
      newConnection.friendID = conn[1].friendID;
      population[i].conn[newConnection.friendID] = newConnection;
      newConnection.friendID = i;
      addConnection[conn[1].friendID].push_back(newConnection);
    }
    //remove deleteNum number of connections that have largest like/dislike 
    for(size_t k = population.size()-1; k >= population.size() - deleteNum; k--){
      newConnection.like = 0;
      newConnection.friendID = conn[k].friendID;
      population[k].conn[newConnection.friendID] = newConnection;
      newConnection.friendID = i;
      reduceConnection[conn[k].friendID].push_back(newConnection);
    }
  }  
  for(size_t i = 0; i < population.size(); i++){
    std::vector<Connection> add = addConnection[i];
    for(size_t p = 0; p < add.size(); p++){
      Connection val = add[p];
      population[i].conn[val.friendID].like = val.like;
    }

    std::vector<Connection> drop = reduceConnection[i];
    for(size_t q = 0; q < drop.size(); q++){
      Connection val = drop[q];
      population[i].conn[val.friendID].like = 0;
    }
  }
}

// These functions are marked inline only because we want to define them in the
// header file.
inline bool loadFromFile(std::string fileName,
                         std::vector<People> &population) {

  std::ifstream f(fileName);
  assert((bool)f && "Cannot open input file");

  std::string line;
  int personID = 0;
  while (std::getline(f, line)) {
    People person;
    std::stringstream sstream(line);
    std::string str;
    int count = 0;
    while(std::getline(sstream, str, ',')){
        Connection conn;
        conn.friendID = count;
        conn.like =std::stof(str);
        person.conn.push_back(conn);
        count += 1;
    }
    person.eval = (rand()%10 + 0.0)/10;
    person.id = personID;
    personID += 1;
    population.push_back(person);
  }
  return true;
}

inline void saveToFile(std::string fileName,
                       const std::vector<People> &population) {
  std::ofstream f(fileName);
  assert((bool)f && "Cannot open output file");

  f << std::setprecision(9);
  
  for (auto p : population) {
    for (auto connection: p.conn){
        f << connection.like << ",";
    }
    f << std::endl;
  }
  assert((bool)f && "Failed to write to output file");
}

inline void saveToResult(std::string fileName, const std::vector<People> &population){
  std::ofstream f(fileName);
  assert((bool)f && "Cannot open output file");

  f << std::setprecision(9);
  
  for (auto p : population) {
    f << p.eval << std::endl;
  }
  assert((bool)f && "Failed to write to output file");
}


#endif
