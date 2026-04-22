#include <string>
#include <fstream>
#include "../include/table.hpp"
#include "../include/random.hpp"
#include "../include/philosopher.hpp"

#ifndef USE_PHILOSOPHER
#define USE_PHILOSOPHER Philosopher
#endif

int main(int argc, char** argv) {
  if (argc <= 1)
    return -1;

  std::ifstream graph_file(argv[1]);
  std::string line;

  std::getline(graph_file, line);
  const int size = (line.size() + 2) / 3;

  int init_seed = 214;
  SFC64 init_rng(init_seed);

  Table table;
  for (int i = 0; i < size; i++) {
    table.AddNewNode(std::make_unique<USE_PHILOSOPHER>(USE_PHILOSOPHER(init_rng.Next())));
  }

  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (line[3*j] - '0' == 1)
        table.AddEdge(i, j);
    }
    std::getline(graph_file, line);
  }

  table.StartSimulation();
  
  //Change this for the actual needs of the simulation (aka how many times each philosopher drinks)
  sleep(5);

  table.StopSimulation();
  table.DisplayStats();

  return 0;
}
