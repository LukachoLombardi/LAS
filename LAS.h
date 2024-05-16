#pragma once
#include <Arduino.h>
#include <Logger.h>

#include "LASMacros.h"
#include "Version.h"

#include "Callable.h"
#include "LASConfig.h"

namespace LASUtils {
#include "Task.h"

struct DummyTask : Task {
  bool isActive = false;  //whether or not a Task is actively maintained. inactive tasks are invalid and can be deleted or overwritten at any time.
  Callable* callable = nullptr;
  bool deleteAfter = true;
  long triggerTime = 0;  //time in millis to next trigger func
  bool repeat = false;   //if true: adds repeatIntervall to triggerTime instead of deactivating task right before execution, creating a recurring task.
  int repeatInterval = 0;
  int remainingRepeats = 0;
};

class CallableVoidFunction : public Callable {
  void (*func)();
public:
  void run();
  CallableVoidFunction(void (*funcIn)());
};
}

class LAS {
private:
  LASConfig config;
  Task* schedule;
  /*{ Task{
    false,
    nullptr,
    true,
    0,
    false,
    0 } }; */

  int activeTaskIndex = 0;
  Logger logger;
  bool schedulerInitialized = false;
  static bool schedulerRunning;

  static int determineFirstInactiveIndex(Task array[], int length);

  void finishTask(Task* task);

  char* taskToCharStr(Task* task);

public:
  int getActiveTaskIndex();
  Task getActiveTask();
  Task getTask(int index);

  void scheduleFunction(void (*func)(), long triggerTime = ASAP, bool deleteAfter = true, bool repeat = false, int repeatInterval = 0, int remainingRepeats = -1);
  void scheduleCallable(Callable* callable, long triggerTime = ASAP, bool deleteAfter = true, bool repeat = false, int repeatInterval = 0, int remainingRepeats = -1);
  void scheduleIn(void (*func)(), long triggerDelay, bool deleteAfter = true);
  void scheduleIn(Callable* callable, long triggerDelay, bool deleteAfter = true);
  void scheduleRepeated(void (*func)(), int repeatInterval = ASAP, int repeats = -1, bool deleteAfter = true);
  void scheduleRepeated(Callable* callable, int repeatInterval = ASAP, int repeats = -1, bool deleteAfter = true);

  void printWelcome();
  char* scheduleToCharStr();  //WARNING: VERY MEMORY HUNGRY, WILL PROBABLY CRASH YOUR ARDUINO
  void printSchedule();
  void startScheduler();
  void clearSchedule();

  void initScheduler(LASConfig config, Logger logger);

  ~LAS();
};
