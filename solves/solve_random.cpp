#include <string>
#include <vector>
#include <cstdint>
#include "../include/table.hpp"
#include "../include/philosopher.hpp"

class Philosopher_Random : public Philosopher {
  private:
    std::vector<uint64_t> owned_bottles;
  protected:
    void RequestBottles() override {
      bool has_bottles = false;
      for (auto&& neig_bot : channels) {
        if (neig_bot.second->mutex.try_lock())
          owned_bottles.push_back(neig_bot.first);
        if (owned_bottles.size() >= wanted_bottles) {
          has_bottles = true;
          break;
        }
      }

      if (!has_bottles) {
        for (auto&& bot_idx : owned_bottles) {
          channels[bot_idx]->mutex.unlock();
          sleep(rng.NextInt(0, neighboors));
        }
        owned_bottles.clear();
      }

      acquired_bottles = owned_bottles.size();
    }

    void ReleaseBottles() override {
      for (auto&& bot_idx : owned_bottles)
        channels[bot_idx]->mutex.unlock();
      owned_bottles.clear();
    }
  public:
    Philosopher_Random(uint64_t seed) : Philosopher(seed) {}
};

#define PHILOSOPHER_CLASS Philosopher_Random
#include "../include/test.hpp"

int main(int argc, char** argv) {
  if (argc < 3)
    return 0;
  
  Table table;
  test_solution(&table, std::string(argv[1]), atoi(argv[2]));
}
