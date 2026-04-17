#pragma once

#include <vector>
#include <chrono>
#include <cstdint>
#include <pthread.h>
#include <unordered_map>

#include "bottle.hpp"
#include "random.hpp"

enum class State {
  TRANQUILO,
  SEDE,
  BEBENDO
};

class Philosopher {
  private:
    pthread_t thread_id;

    uint64_t neighboors;
    uint64_t wanted_bottles;
    uint64_t acquired_bottles;
    std::unordered_map<uint64_t, Bottle*> channels;
    
    double time_thirsty;
    double time_until_state_change;
    double time_elapsed_since_last_loop;
    double total_time_thirsty;
    
    SFC64 rng;
    State current_state;

    static void * ThreadJob(void * arg) {
      Philosopher * this_ptr = static_cast<Philosopher *>(arg);

      auto last_time_check = std::chrono::steady_clock::now();
      for (;;) {
        auto current_time_check = std::chrono::steady_clock::now();
        
        this_ptr->time_elapsed_since_last_loop = std::chrono::duration<double>(current_time_check - last_time_check).count();
        this_ptr->StateMachine();

        last_time_check = current_time_check;
      } 

      return NULL;
    }

    void StateMachine() {
      switch (current_state) {
        case State::TRANQUILO:
          if (time_until_state_change > 0) {
            time_until_state_change -= time_elapsed_since_last_loop;
          }
          else {
            current_state = State::SEDE;
            wanted_bottles = rng.NextInt(2, neighboors);
          }
          break;
        case State::SEDE:
          RequestBottles();
          if (acquired_bottles < wanted_bottles) {
            time_thirsty += time_elapsed_since_last_loop;
          }
          else {
            current_state = State::BEBENDO;
            total_time_thirsty += time_thirsty;
            time_thirsty = 0.0;
            time_until_state_change = 1.0;
            ReleaseBottles();
          }
          break;
        case State::BEBENDO:
          if (time_until_state_change > 0) {
            time_until_state_change -= time_elapsed_since_last_loop;
          }
          else {
            current_state = State::TRANQUILO;
            time_until_state_change = rng.NextInt(0, neighboors);
          }
          break;
      }
    }

  protected:
    virtual void RequestBottles() = 0;
    virtual void ReleaseBottles() = 0;

  public:
    uint64_t idx;
    std::vector<uint64_t> adj_list;

    Philosopher(uint64_t seed) : 
      neighboors(0),
      wanted_bottles(0),
      channels(),
      time_thirsty(0),
      time_until_state_change(0),
      rng(seed),
      current_state(State::TRANQUILO) {}

    void AddNeighboor(uint64_t idx, Bottle * ch) {
      adj_list.push_back(idx);
      channels[idx] = ch;
      neighboors++;
    }

    void StartThreadJob() {
      std::chrono::steady_clock::now();
      time_until_state_change = (double)rng.NextInt(0, neighboors);
      pthread_create(&thread_id, NULL, ThreadJob, this);
    }
};
