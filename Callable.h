#pragma once
#include "Task.h"

class Callable {
public:
  virtual void run() = 0;
  void onFinish();

  void operator()();

  void finish();

  Task* taskPtr = nullptr;  //check for nullptr to ensure correct scheduler environment
};
