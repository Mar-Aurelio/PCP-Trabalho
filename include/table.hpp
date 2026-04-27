#pragma once

#include <memory>
#include <unistd.h>
#include <vector>
#include <utility>
#include <iomanip>
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
        std::cout << '\n';
        bool is_done = true;
        for (auto&& philosopher : philosophers) {
          is_done = is_done && philosopher->drink_counter >= solution_constrain; 
        }
        is_satisfied = is_done;

        // ROW 1: Print Indices
        for (size_t i = 0; i < philosophers.size(); ++i) {
          std::cout << std::setw(7) << i << "  ";
        }
        std::cout << '\n';

        // ROW 2: Print Colored Circles
        for (auto&& philosopher : philosophers) {
          State s = philosopher->GetState();
          std::string color_code;

          if (s == State::TRANQUILO) color_code = "\033[32m";      // Green
          else if (s == State::SEDE) color_code = "\033[31m";      // Red
          else if (s == State::BEBENDO) color_code = "\033[33m";   // Yellow

          char done = (philosopher->drink_counter >= solution_constrain) ? '!' : ' ';
          // Print 6 spaces to center the circle under the 7-character width index
          std::cout << std::setw(5) << philosopher->drink_counter 
            << done << color_code << "●" << "\033[0m  ";
        }
        std::cout << '\n';

        // ROW 3: Print Total Time Thirsty (up to 2 decimal places)
        for (auto&& philosopher : philosophers) {
          std::cout << std::setw(7) << std::fixed << std::setprecision(2) 
            << philosopher->GetTotalTimeThirsty() << "  ";
        }
        std::cout << "\n\n";
        if (!is_satisfied)
          std::cout << "\033[5A" << "\033[0J";
        usleep(500);
      }

      for (auto&& philosopher : philosophers)
        philosopher->StopThreadJob();
    }
};
