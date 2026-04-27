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
      bool got_new_bottle = false;
      for (auto&& neig_bot : channels) {
        if (neig_bot.second->mutex.try_lock()) {
          owned_bottles.push_back(neig_bot.first);
          got_new_bottle = true;
          break;
        }
      }

      bool has_enough_bottles = false;
      if (owned_bottles.size() >= wanted_bottles)
        has_enough_bottles = true;

      if (!has_enough_bottles && !got_new_bottle) {
        ReleaseBottles();
        sleep(rng.NextInt(0, neighboors));
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
