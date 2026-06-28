#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <memory>
#include "../include/table.hpp"
#include "../include/philosopher.hpp"
#include "../include/random.hpp"

struct BottleState {
  bool have_bottle     = false;
  bool is_dirty        = true;
  bool pending_request = false;
};

class Philosopher_CM : public Philosopher {
  private:
    uint64_t my_id;
    std::unordered_map<uint64_t, BottleState> bstate;
    std::mutex              mtx;
    std::condition_variable cv;
    std::unordered_map<uint64_t, Philosopher_CM*> neighbors;

    uint64_t countBottles() {
      uint64_t count = 0;
      for (auto& [id, bs] : bstate)
        if (bs.have_bottle) count++;
      return count;
    }

    void sendRequests() {
      // Coleta quem pedir fora do lock para evitar deadlock
      std::vector<std::pair<uint64_t, Philosopher_CM*>> to_request;
      {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& [id, bs] : bstate)
          if (!bs.have_bottle)
            to_request.push_back({id, neighbors[id]});
      }
      for (auto& [id, nb] : to_request)
        nb->OnRequest(my_id);
    }

  public:
    Philosopher_CM(uint64_t seed, uint64_t id)
      : Philosopher(seed), my_id(id) {}

    void AddNeighbor(uint64_t id, Philosopher_CM* ptr) {
      neighbors[id] = ptr;
    }

    void Init() {
      std::lock_guard<std::mutex> lock(mtx);
      for (auto& [id, bottle] : channels) {
        auto& bs           = bstate[id];
        bs.is_dirty        = true;
        bs.have_bottle     = (my_id < id);
        bs.pending_request = false;
      }
    }

    void OnRequest(uint64_t requester_id) {
      Philosopher_CM* nb    = nullptr;
      bool should_notify    = false;
      {
        std::lock_guard<std::mutex> lock(mtx);
        auto& bs = bstate[requester_id];
        if (!bs.have_bottle) return;

        if (bs.is_dirty) {
          bs.is_dirty    = false;
          bs.have_bottle = false;
          nb             = neighbors[requester_id];
          should_notify  = true;
        } else {
          bs.pending_request = true;
        }
      }
      if (should_notify)
        nb->OnReceive(my_id);
    }

    void OnReceive(uint64_t sender_id) {
      {
        std::lock_guard<std::mutex> lock(mtx);
        bstate[sender_id].have_bottle = true;
      }
      cv.notify_one();
    }

  protected:
    void RequestBottles() override {
        if (wanted_bottles == 0) return;

        auto start = std::chrono::steady_clock::now();

        while (true) {
            sendRequests();

            std::unique_lock<std::mutex> lock(mtx);
            cv.wait_for(lock,
            std::chrono::milliseconds(100),
            [this] { return countBottles() >= wanted_bottles; }
            );

            if (countBottles() >= wanted_bottles) {
            acquired_bottles = wanted_bottles;

            // Mede e registra o tempo de espera manualmente
            auto end = std::chrono::steady_clock::now();
            double waited = std::chrono::duration<double>(end - start).count();
            addThirstyTime(waited);  // injeta no acumulador da classe base

            return;
            }
        }
    }
    void ReleaseBottles() override {
      std::vector<std::pair<uint64_t, Philosopher_CM*>> to_notify;
      {
        std::lock_guard<std::mutex> lock(mtx);
        for (auto& [id, bs] : bstate) {
          bs.is_dirty = true;
          if (bs.pending_request) {
            bs.pending_request = false;
            bs.have_bottle     = false;
            to_notify.push_back({id, neighbors[id]});
          }
        }
      }
      for (auto& [id, nb] : to_notify)
        nb->OnReceive(my_id);
    }
};

int main(int argc, char** argv) {
#if defined(_WIN32)
  system("chcp 65001 > nul");
#endif
  if (argc < 3) return 0;

  std::ifstream f1(argv[1]);
  std::string line;
  std::getline(f1, line);
  const int size = (line.size() + 2) / 3;
  f1.close();

  SFC64 rng(214);
  std::vector<Philosopher_CM*> phils;
  Table table;

  for (int i = 0; i < size; i++) {
    auto p = std::make_unique<Philosopher_CM>(rng.Next(), (uint64_t)i);
    phils.push_back(p.get());
    table.AddNewNode(std::move(p));
  }

  std::ifstream f2(argv[1]);
  std::getline(f2, line);
  for (int i = 0; i < size; i++) {
    for (int j = i + 1; j < size; j++) {
      if (line[3*j] - '0' == 1) {
        table.AddEdge(i, j);
        phils[i]->AddNeighbor(j, phils[j]);
        phils[j]->AddNeighbor(i, phils[i]);
      }
    }
    std::getline(f2, line);
  }
  f2.close();

  for (int i = 0; i < size; i++)
    phils[i]->Init();

  table.Simulate(atoi(argv[2]));
  return 0;
}