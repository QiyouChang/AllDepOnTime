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

struct StartupOptions {
  int numIterations = 1;
  int numPeople = 5;
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

inline People randomizedPeople(const People &pi, float score, int addNum, int deleteNum, int totalNumPeople){
    //drop some connection and add some connection 
    People newPerson = pi;
    //TO DO: NEED finish up the function.
    return newPerson;
}

// These functions are marked inline only because we want to define them in the
// header file.
inline bool loadFromFile(std::string fileName,
                         std::vector<People> &population) {
  std::ifstream f(fileName);
  assert((bool)f && "Cannot open input file");

  std::string line;
  while (std::getline(f, line)) {
    People person;
    std::stringstream sstream(line);
    std::string str;
    
    while(std::getline(sstream, str, ' ')){
        Connection conn;
        size_t pos =  str.find(",");
        size_t size = str.size();
        conn.friendID = (int)str.substr(0, pos);
        conn.like = str.substr(pos+1, size-pos-1);
        population.push_back(conn);
    }
    person.id = (int)population.size();
  }
  return true;
}

inline void saveToFile(std::string fileName,
                       const std::vector<People> &population) {
  std::ofstream f(fileName);
  assert((bool)f && "Cannot open output file");

  f << std::setprecision(9);
  
  for (auto p : population) {
    std::string line;
    for (auto connection: p.conn){
        f << connection.friendID << "," << connection.like << " ";
    }
    f << std::endl;
  }
  assert((bool)f && "Failed to write to output file");
}

#endif
