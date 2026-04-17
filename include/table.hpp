#pragma once

#include <memory>
#include <utility>
#include <vector>
#include <iostream>

#include "philosopher.hpp"
#include "bottle.hpp"

class Table {
  private:
    std::vector<std::unique_ptr<Philosopher>> philosophers;
    std::vector<std::unique_ptr<Bottle>> bottles;

  public:
    Table() : philosophers(), bottles() {}

    int AddNewNode(std::unique_ptr<Philosopher> new_p) {
      new_p->idx = philosophers.size();
      philosophers.push_back(std::move(new_p));

      return new_p->idx;
    }

    void AddEdge(int idx1, int idx2) {
      std::unique_ptr<Bottle> new_bottle = std::make_unique<Bottle>();
      philosophers[idx1]->AddNeighboor(idx2, new_bottle.get());
      philosophers[idx2]->AddNeighboor(idx1, new_bottle.get());
      bottles.push_back(std::move(new_bottle));
    }

    void DisplayStructure() {
      for (auto&& p : philosophers) {
        std::cout << p->idx << " -> ";
        for (int adj : p->adj_list)
          std::cout << adj << ", ";
        std::cout << '\n';
      }
    }
};
