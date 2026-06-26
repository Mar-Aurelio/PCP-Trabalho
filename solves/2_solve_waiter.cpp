#include <string>
#include <vector>
#include <cstdint>
#include <semaphore.h>          


#include "../include/table.hpp"
#include "../include/philosopher.hpp"

static sem_t waiter_semaphore;
static bool waiter_semaphore_initialized = false;
static int waiter_capacity = 0;

class Philosopher_Random : public Philosopher {
  private:
    std::vector<uint64_t> owned_bottles;
  protected:
    void RequestBottles() override {
      sem_trywait(&waiter_semaphore);
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
        ReleaseBottles();
        sleep(rng.NextInt(0, neighboors));
      }

      sem_post(&waiter_semaphore); //signal do semáforo
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
  #if defined(_WIN32)
    system("chcp 65001 > nul");  // força UTF-8 no Windows
  #endif
  if (argc < 3)
    return 0;
  

  // Lê o número de filósofos do arquivo antes de inicializar
  std::ifstream f(argv[1]);
  std::string line;
  std::getline(f, line);
  int n_philosophers = (line.size() + 2) / 3;
  f.close();

  //inicialização do semáforo garçom
  waiter_capacity = n_philosophers-1;
  if (!waiter_semaphore_initialized) {
    sem_init(&waiter_semaphore, 0, waiter_capacity);
    waiter_semaphore_initialized = true;
  }

  Table table;
  test_solution(&table, std::string(argv[1]), atoi(argv[2]));
  sem_destroy(&waiter_semaphore);
}
