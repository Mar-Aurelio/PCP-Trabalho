#pragma once

#include <vector>
#include <pthread.h>

#include "random.hpp"

enum class State {
  TRANQUILO,
  SEDE,
  BEBENDO
};

class Philosopher {
  private:
    pthread_t thread_id;
    unsigned int neighboors;
    unsigned int wanted_bottles;
    double time_thirsty;
    double time_waiting;
    SFC64 rng;
    State current_state;
    std::vector<unsigned int> holding_bottles;

    static void * ThreadJob(void * arg) {
      Philosopher * this_ptr = static_cast<Philosopher*>(arg);

      for (;;) {
        this_ptr->StateMachine();
      }

      return NULL;
    }

    virtual void StateMachine();

  public:
    unsigned long idx;
    std::vector<unsigned int> adj_list;

    Philosopher(unsigned long seed) : 
      neighboors(0),
      wanted_bottles(0),
      time_thirsty(0),
      time_waiting(0),
      rng(seed),
      current_state(State::TRANQUILO) {}

    void AddNeighboor(unsigned int idx) {
      adj_list.push_back(idx);
      neighboors++;
    }

    void StartThreadJob() {
      time_waiting = (double)rng.NextInt(0, neighboors);
      pthread_create(&thread_id, NULL, ThreadJob, this);
    }
};
