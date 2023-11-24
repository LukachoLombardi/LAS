#pragma once
#include <Arduino.h>
#include "LASConfig.h"
#include "Version.h"
#include <Logger.h>

namespace LAS {

struct Task;
class Callable {
  public:
    virtual void run() = 0;
    void onFinish();

    void operator()();

    Task *taskPtr = nullptr; //check for nullptr to ensure correct scheduler environment
};

class CallableVoidFunction : public Callable{
  void (*func)();
  public:
    void run();
    CallableVoidFunction(void (*funcIn)());
};

struct Task {
  bool isActive;     //whether or not a Task is actively maintained. inactive tasks are invalid and can be deleted or overwritten at any time.
  Callable* callable;
  bool deleteAfter;
  long triggerTime;  //time in millis to next trigger func
  bool repeat;  //if true: adds repeatIntervall to triggerTime instead of deactivating task right before execution, creating a recurring task.
  int repeatInterval;
  int remainingRepeats;
};

struct DummyTask: Task {
  bool isActive = false;     //whether or not a Task is actively maintained. inactive tasks are invalid and can be deleted or overwritten at any time.
  Callable* callable = nullptr;
  bool deleteAfter = true;
  long triggerTime = 0;  //time in millis to next trigger func
  bool repeat = false;  //if true: adds repeatIntervall to triggerTime instead of deactivating task right before execution, creating a recurring task.
  int repeatInterval = 0;
  int remainingRepeats = 0;
};

extern Task schedule[SCHEDULE_SIZE];
extern int activeTaskIndex;
extern Logger logger;
extern bool schedulerInitialized;
extern bool schedulerRunning;

int getActiveTaskIndex();
Task getActiveTask();
Task getTask(int index);

int determineFirstFreeIndex(Task array[], int length);

void scheduleFunction(void (*func)(), long triggerTime = ASAP, bool deleteAfter = true, bool repeat = false, int repeatInterval = 0, int remainingRepeats = -1);
void scheduleCallable(Callable* callable, long triggerTime = ASAP, bool deleteAfter = true, bool repeat = false, int repeatInterval = 0, int remainingRepeats = -1);
void scheduleIn(void (*func)(), long triggerDelay);
void scheduleIn(Callable* callable, long triggerDelay);
void scheduleRepeated(void (*func)(), int repeatInterval = ASAP, int repeats = -1);
void scheduleRepeated(Callable* callable, int repeatInterval = ASAP, int repeats = -1);

void printWelcome();
char* taskToCharStr(Task task);
char* scheduleToCharStr();  //WARNING: VERY MEMORY HUNGRY, WILL PROBABLY CRASH YOUR ARDUINO
void printSchedule();
void initScheduler(Logger logger);
void initScheduler();
void startScheduler();
void clearSchedule();
}
