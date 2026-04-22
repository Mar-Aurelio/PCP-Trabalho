#pragma once

#include <memory>
#include <vector>
#include <utility>
#include <iostream>

#include "bottle.hpp"
#include "philosopher.hpp"

class Table {
  private:
    std::vector<std::unique_ptr<Philosopher>> philosophers;
    std::vector<std::unique_ptr<Bottle>> bottles;

  public:
    Table() : philosophers(), bottles() {}

    void AddNewNode(std::unique_ptr<Philosopher> new_p) {
      new_p->idx = philosophers.size();
      philosophers.push_back(std::move(new_p));
    }

    void AddEdge(int idx1, int idx2) {
      bottles.push_back(std::make_unique<Bottle>());
      const int bot_idx = bottles.size() - 1;
      philosophers[idx1]->AddNeighboor(idx2, bottles[bot_idx].get());
      philosophers[idx2]->AddNeighboor(idx1, bottles[bot_idx].get());
    }

    void StartSimulation() {
      for (auto&& p : philosophers)
        p->StartThreadJob();
    }

    void StopSimulation() {
      for (auto&& p : philosophers)
        p->StopThreadJob();
    }

    void DisplayStats() {
      for (auto&& p : philosophers) {
        std::cout << "Index: " << p->idx << '\n';
        p->DisplayStats();
      }
    }

    void DisplayStructure() {
      for (auto&& p : philosophers) {
        std::cout << p->idx << " -> ";
        for (auto&& adj : p->adj_list)
          std::cout << adj << ", ";
        std::cout << '\n';
      }
    }
};
