#pragma once

#include <vector>
#include <iostream>
#include <unordered_map>

#include "philosopher.hpp"

class Table {
  private:
    std::vector<Philosopher> philosophers;

  public:
    Table() : philosophers() {}

    int AddNewNode(Philosopher new_p) {
      new_p.idx = philosophers.size();
      philosophers.push_back(new_p);

      return new_p.idx;
    }

    void AddEdge(int idx1, int idx2) {
      philosophers[idx1].AddNeighboor(idx2);
      philosophers[idx2].AddNeighboor(idx1);
    }

    void DisplayStructure() {
      for (Philosopher philosopher : philosophers) {
        std::cout << philosopher.idx << " -> ";
        for (int adj : philosopher.adj_list)
          std::cout << adj << ", ";
        std::cout << '\n';
      }
    }
};
