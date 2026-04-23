#include <string>
#include "table.hpp"

void test_solution(Table * table, std::string file_name, int solution_constrain);

#ifdef PHILOSOPHER_CLASS
#include <fstream>
#include "random.hpp"

void test_solution(Table * table, std::string file_name, int solution_constrain) {
  std::ifstream graph_file(file_name);
  std::string line;

  std::getline(graph_file, line);
  const int size = (line.size() + 2) / 3;

  int init_seed = 214;
  SFC64 init_rng(init_seed);

  for (int i = 0; i < size; i++) {
    table->AddNewNode(std::make_unique<PHILOSOPHER_CLASS>(PHILOSOPHER_CLASS(init_rng.Next())));
  }

  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (line[3*j] - '0' == 1)
        table->AddEdge(i, j);
    }
    std::getline(graph_file, line);
  }

  table->Simulate(solution_constrain);
}
#endif
