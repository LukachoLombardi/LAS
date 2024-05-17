#include "LAS.h"

using namespace LASUtils;

bool LAS::schedulerRunning = false;

void CallableVoidFunction::run() {
  this->func();
}
CallableVoidFunction::CallableVoidFunction(void (*funcIn)()) {
  this->func = funcIn;
}

int LAS::getActiveTaskIndex() {
  return activeTaskIndex;
}
Task LAS::getActiveTask() {
  return getTask(activeTaskIndex);
}

int LAS::determineFirstInactiveIndex(Task array[], int length) {
  for (int index = 0; index < length; index++) {
    if (!array[index].isActive) {
      return index;
    }
  }
  return length + 1;
}

void LAS::scheduleCallable(Callable *callable, long triggerTime, bool deleteAfter, bool repeat, int repeatInterval, int remainingRepeats) {
  if (!LAS::schedulerInitialized) {
    Serial.println("SCHEDULER/LOGGER NOT INITIALIZED. RUN initScheduler() FIRST!");
    return;
  }
  Task newTask = Task{
    true,
    callable,
    deleteAfter,
    max(ASAP, triggerTime),
    repeat,
    max(ASAP, repeatInterval),
    max(-1, remainingRepeats)
  };
  int freeIndex = LAS::determineFirstInactiveIndex(schedule, config.scheduleSize);
  if (freeIndex > config.scheduleSize) {
    logger.printline("SCHEDULE IS FULL! ABORTING TO AVOID UNDEFINED BEHAVIOUR.", "severe");
    //abort
    while (true)
      ;
  }
  schedule[freeIndex] = newTask;
  schedule[freeIndex].callable->taskPtr = &schedule[freeIndex];
  char buffer[config.strSize / 2] = "";
  snprintf(buffer, sizeof(buffer), "scheduled Task at %p", &schedule[freeIndex]);
  logger.printline(buffer, logger.LogLevel::Debug);
}

void LAS::scheduleFunction(void (*func)(), long triggerTime, bool deleteAfter, bool repeat, int repeatInterval, int remainingRepeats) {
  scheduleCallable(new CallableVoidFunction(func), triggerTime, deleteAfter, repeat, repeatInterval, remainingRepeats);
}

void LAS::scheduleIn(void (*func)(), long triggerDelay, bool deleteAfter) {
  scheduleFunction(func, millis() + triggerDelay, deleteAfter);
}

void LAS::scheduleIn(Callable *callable, long triggerDelay, bool deleteAfter) {
  scheduleCallable(callable, millis() + triggerDelay, deleteAfter);
}

void LAS::scheduleRepeated(void (*func)(), int repeatInterval, int repeats, bool deleteAfter) {
  scheduleFunction(func, millis() + repeatInterval, deleteAfter, true, repeatInterval, repeats);
}

void LAS::scheduleRepeated(Callable *callable, int repeatInterval, int repeats, bool deleteAfter) {
  scheduleCallable(callable, millis() + repeatInterval, deleteAfter, true, repeatInterval, repeats);
}

void LAS::printWelcome() {
  Serial.println("This Unit is running");
  Serial.println(" __       ______  ____       ");
  Serial.println("/\\ \\     /\\  _  \\/\\  _`\\     ");
  Serial.println("\\ \\ \\    \\ \\ \\L\\ \\ \\,\\L\\_\\   ");
  Serial.println(" \\ \\ \\  __\\ \\  __ \\/_\\__ \\   ");
  Serial.println("  \\ \\ \\L\\ \\\\ \\ \\/\\ \\/\\ \\L\\ \\ ");
  Serial.println("   \\ \\____/ \\ \\_\\ \\_\\ `\\____\\\\");
  Serial.println("    \\/___/   \\/_/\\/_/\\/_____/");
  Serial.println("");
  Serial.print("Lukacho's Amazing Scheduler - alpha ");
  Serial.print(LAS_VERSION);
  Serial.println(" - now with Callables!");
  Serial.println();
}

void LAS::finishTask(Task *task) {
  task->isActive = false;
  task->callable->onFinish();
  if (task->deleteAfter) {
    delete task->callable;
    task->callable = nullptr;
  }
}

void LAS::startScheduler() {
  logger.printline("starting scheduler...");
  if (!LAS::schedulerInitialized) {
    Serial.println("SCHEDULER/LOGGER NOT INITIALIZED. RUN initScheduler() FIRST!");
    return;
  }
  printWelcome();
  if (schedule[0].isActive && schedule[0].triggerTime != ASAP) {
    logger.printline("Please consider adding initial Tasks through an ASAP Task for high precision apps", logger.LogLevel::Warning);
  }
  if (LAS::schedulerRunning) {
    logger.printline("THERE MIGHT BE ANOTHER SCHEDULER ALREADY RUNNING! ABORTING...", logger.LogLevel::Severe);
    return;
  } else {
    LAS::schedulerRunning = true;
  }
  while (true) {
    for (int index = 0; index < config.scheduleSize; index++) {
      activeTaskIndex = index;
      Task currentTask = schedule[index];
      if ((currentTask.isActive) && (currentTask.callable != nullptr) && (currentTask.triggerTime <= millis())) {

        if (config.lagWarning && millis() - currentTask.triggerTime >= config.maxLagMs && currentTask.triggerTime != 0) {
          logger.printline("SCHEDULER IS FALLING BEHIND CRITICALLY!", logger.LogLevel::Warning);
        }

        currentTask.callable->run();

        if (currentTask.repeat) {
          schedule[index].triggerTime = millis() + currentTask.repeatInterval;
          if (schedule[index].remainingRepeats == 0) {
            finishTask(&schedule[index]);
            char buffer[config.strSize / 2] = "";
            snprintf(buffer, sizeof(buffer), "finished repeat Task at %p", &schedule[index]);
            logger.printline(buffer, logger.LogLevel::Debug);
          } else if (currentTask.remainingRepeats != ENDLESS_LOOP) {
            schedule[index].remainingRepeats--;
          }
        } else {
          finishTask(&schedule[index]);
          char buffer[config.strSize / 2] = "";
          snprintf(buffer, sizeof(buffer), "finished Task at %p", &schedule[index]);
          logger.printline(buffer, logger.LogLevel::Debug);
        }
      } else if (currentTask.callable != nullptr && !currentTask.isActive && currentTask.deleteAfter) {
        delete currentTask.callable;
        currentTask.callable = nullptr;
      }
    }
  }
}

void LAS::initScheduler(LASConfig config, Logger logger) {
  interrupts();
  logger = logger;
  config = config;

  schedule = new Task[config.scheduleSize]{ Task{
    false,
    nullptr,
    true,
    0,
    false,
    0 } };

  schedulerInitialized = true;
  logger.printline("scheduler initialized");
  logger.printline("logger test", logger.LogLevel::Debug);
}

LAS::~LAS() {
  LAS::schedulerRunning = false;
  clearSchedule();
  delete [] schedule;
}

void LAS::clearSchedule() {
  logger.printline("Clearing schedule as demanded programatically.", "warning");
  for (int i = 0; i < config.scheduleSize; i++) {
    finishTask(&schedule[i]);
  }
}

char *LAS::taskToCharStr(Task *task) {
  static char* buffer = new char[config.strSize];
  char buffer_2[config.strSize];
  snprintf(buffer, sizeof(buffer), "Task %p:\n  isActive: %i\n  deleteAfter: %i\n  callable: %p\n  triggerTime: %d\n",
           task, task->isActive, task->deleteAfter, task->callable, task->triggerTime);
  snprintf(buffer_2, sizeof(buffer), "\n  repeat: %i\n  repeatInterval: %i\n  remainingRepeats: %i",
           task->repeat, task->repeatInterval, task->remainingRepeats);
  strcat(buffer, buffer_2);
  return buffer;
}

char *LAS::scheduleToCharStr() {
  static char* buffer = new char[config.strSize];
  for (int index = 0; index < config.scheduleSize; index++) {
    static char indexBuffer[3];
    snprintf(indexBuffer, sizeof(buffer), "\n%d:\n", index);
    strcat(buffer, indexBuffer);
    strcat(buffer, taskToCharStr(&schedule[index]));
  }
  return buffer;
}

Task LAS::getTask(int index) {
  return schedule[index];
}

void LAS::printSchedule() {
  for (int index = 0; index < config.scheduleSize; index++) {
    char buffer[config.strSize];
    snprintf(buffer, sizeof(buffer), "%d:", index);
    logger.printline(buffer);
    logger.printline(taskToCharStr(&schedule[index]));
  }
}
