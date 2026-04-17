#pragma once

#include <mutex>

struct Bottle {
  std::mutex mutex;
  bool has_bottle;
  bool has_request;
};
