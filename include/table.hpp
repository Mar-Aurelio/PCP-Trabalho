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
      philosophers.push_back(std::move(new_p));
    }

    void AddEdge(int idx1, int idx2) {
      bottles.push_back(std::make_unique<Bottle>());
      const int bot_idx = bottles.size() - 1;
      philosophers[idx1]->AddNeighboor(idx2, bottles[bot_idx].get());
      philosophers[idx2]->AddNeighboor(idx1, bottles[bot_idx].get());
    }

    void Simulate(int solution_constrain) {
      for (auto&& philosopher : philosophers)
        philosopher->StartThreadJob();

      bool is_satisfied = false;
      while (!is_satisfied) {
        bool is_done = true;
        for (auto&& philosopher : philosophers) {
          is_done = is_done && philosopher->drink_counter >= solution_constrain; 
        }
        is_satisfied = is_done;

        int i = 0;
        for (auto&& philosopher : philosophers) {
          std::cout << "Philosopher id: " << i++ << '\n';
          philosopher->DisplayStats();
        }
        if (!is_satisfied)
          std::cout << "\033[" << 5*i << "A\r";
        sleep(1);
      }

      for (auto&& philosopher : philosophers)
        philosopher->StopThreadJob();
    }
};
