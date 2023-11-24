#include "LAS.h"

namespace LAS {
Task schedule[SCHEDULE_SIZE] = { Task{
  false,
  nullptr,
  true,
  0,
  false,
  0 } };

int activeTaskIndex = 0;
bool schedulerInitialized = false;
extern bool schedulerRunning = false;
Logger logger;

void CallableVoidFunction::run(){
  this->func();
}
CallableVoidFunction::CallableVoidFunction(void (*funcIn)()){
  this->func = funcIn;
}
void Callable::operator()(){
  return run();
}

void Callable::onFinish(){}

int getActiveTaskIndex() {
  return activeTaskIndex;
}
Task getActiveTask() {
  return getTask(activeTaskIndex);
}

int determineFirstInactiveIndex(Task array[], int length) {
  for (int index = 0; index < length; index++) {
    if (!array[index].isActive) {
      return index;
    }
  }
  return length+1;
}

void scheduleCallable(Callable *callable, long triggerTime, bool deleteAfter, bool repeat, int repeatInterval, int remainingRepeats) {
  if(!schedulerInitialized){
    Serial.println("SCHEDULER/LOGGER NOT INITIALIZED. RUN initScheduler() FIRST!");
    return;
  }
  Task newTask = Task{
    true,
    callable,
    deleteAfter,
    triggerTime,
    repeat,
    repeatInterval,
    remainingRepeats
  };
  int freeIndex = determineFirstInactiveIndex(schedule, SCHEDULE_SIZE);
  if(freeIndex > SCHEDULE_SIZE) {
    logger.printline("SCHEDULE IS FULL! ABORTING TO AVOID UNDEFINED BEHAVIOUR.", "severe");
    //abort
    while(true);
  }
  schedule[freeIndex] = newTask;
  schedule[freeIndex].callable->taskPtr = &schedule[freeIndex];
  char buffer[INTERNAL_CHAR_STR_SIZE_UNIT / 2] = "";
  snprintf(buffer, sizeof(buffer), "scheduled Task at %p", &schedule[freeIndex]);
  logger.printline(buffer, logger.LogLevel::Debug);
}

void scheduleFunction(void (*func)(), long triggerTime, bool deleteAfter, bool repeat, int repeatInterval, int remainingRepeats) {
  scheduleCallable(new CallableVoidFunction(func), triggerTime, deleteAfter, repeat, repeatInterval, remainingRepeats);
}

void scheduleIn(void (*func)(), long triggerDelay, bool deleteAfter) {
  scheduleFunction(func, millis() + triggerDelay, deleteAfter);
}

void scheduleIn(Callable *callable, long triggerDelay, bool deleteAfter) {
  scheduleCallable(callable, millis() + triggerDelay, deleteAfter);
}

void scheduleRepeated(void (*func)(), int repeatInterval, int repeats, bool deleteAfter) {
  scheduleFunction(func, millis() + repeatInterval, deleteAfter, true, repeatInterval, repeats);
}

void scheduleRepeated(Callable *callable, int repeatInterval, int repeats, bool deleteAfter) {
  scheduleCallable(callable, millis() + repeatInterval, deleteAfter, true, repeatInterval, repeats);
}

void printWelcome() {
  Serial.println("This Unit is running");
  Serial.println(" __       ______  ____       ");
  Serial.println("/\\ \\     /\\  _  \\/\\  _`\\     ");
  Serial.println("\\ \\ \\    \\ \\ \\L\\ \\ \\,\\L\\_\\   ");
  Serial.println(" \\ \\ \\  __\\ \\  __ \\/_\\__ \\   ");
  Serial.println("  \\ \\ \\L\\ \\\\ \\ \\/\\ \\/\\ \\L\\ \\ ");
  Serial.println("   \\ \\____/ \\ \\_\\ \\_\\ `\\____\\\\");
  Serial.println("    \\/___/   \\/_/\\/_/\\/_____/");
  Serial.println("");
  Serial.print("Lukacho's Amazing Scheduler - alpha "); Serial.print(LAS_VERSION); Serial.println(" - now with Callables!");
  Serial.println();
}

void finishTask(Task task){
  task.isActive = false;
  task.callable->onFinish();
  if(task.deleteAfter){
    delete task.callable;
  }
}

void startScheduler() {
  logger.printline("starting scheduler...");
  if(!schedulerInitialized){
    Serial.println("SCHEDULER/LOGGER NOT INITIALIZED. RUN initScheduler() FIRST!");
    return;
  }
  printWelcome();
  if (schedule[0].isActive && schedule[0].triggerTime != ASAP) {
    logger.printline("Please consider adding initial Tasks through an ASAP Task for high precision apps", logger.LogLevel::Warning);
  }
  if (schedulerRunning) {
    logger.printline("THERE MIGHT BE ANOTHER SCHEDULER ALREADY RUNNING! ABORTING...", logger.LogLevel::Severe);
    return;
  }
  else{
    schedulerRunning = true;
  }
  while (true) {
    for (int index = 0; index < SCHEDULE_SIZE; index++) {
      activeTaskIndex = index;
      Task currentTask = schedule[index];
      if ((currentTask.isActive) && (currentTask.callable != nullptr) && (currentTask.triggerTime <= millis())) {

        if (millis() - currentTask.triggerTime >= CRITICAL_LAG_MS && currentTask.triggerTime != 0) {
          logger.printline("SCHEDULER IS FALLING BEHIND CRITICALLY!", logger.LogLevel::Warning);
        }        

        currentTask.callable->run();

        if (currentTask.repeat) {
          schedule[index].triggerTime = millis() + currentTask.repeatInterval;
          if (currentTask.remainingRepeats != ENDLESS_LOOP) {
            schedule[index].remainingRepeats--;
          }

          if (schedule[index].remainingRepeats == 0) {
            finishTask(schedule[index]);
            char buffer[INTERNAL_CHAR_STR_SIZE_UNIT / 2] = "";
            snprintf(buffer, sizeof(buffer), "finished repeat Task at %p", &schedule[index]);
            logger.printline(buffer, logger.LogLevel::Debug);
          }
        } else {
          finishTask(schedule[index]);
          char buffer[INTERNAL_CHAR_STR_SIZE_UNIT / 2] = "";
          snprintf(buffer, sizeof(buffer), "finished Task at %p", &schedule[index]);
          logger.printline(buffer, logger.LogLevel::Debug);
        }
      }
    }
  }
}

void initScheduler(Logger logger) {
  interrupts();
  logger = logger;
  schedulerInitialized = true;
  logger.printline("logger initialized");
}

void initScheduler() {
  Logger tempLogger = Logger();
  initScheduler(tempLogger);
}

void clearSchedule(){
  logger.printline("Clearing schedule as demanded programatically.", "warning");
  for(int i = 0; i<SCHEDULE_SIZE;i++){
    schedule[i] = DummyTask();
  }
}

char* taskToCharStr(Task *task) {
  static char buffer[INTERNAL_CHAR_STR_SIZE_UNIT];
  snprintf(buffer, sizeof(buffer), "Task %p:\n  isActive: %d\n  callable: %p\n  triggerTime: %d\n  repeat: %d\n  repeatInterval: %d",
           task, task->isActive, task->callable, task->triggerTime, task->repeat, task->repeatInterval);
  return buffer;
}
char* scheduleToCharStr() {
  static char buffer[INTERNAL_CHAR_STR_SIZE_UNIT * SCHEDULE_SIZE];
  for (int index = 0; index < SCHEDULE_SIZE; index++) {
    static char indexBuffer[3];
    snprintf(indexBuffer, sizeof(buffer), "\n%d:\n", index);
    strcat(buffer, indexBuffer);
    strcat(buffer, taskToCharStr(&schedule[index]));
  }
  return buffer;
}

Task getTask(int index) {
  return schedule[index];
}

void printSchedule() {
  for (int index = 0; index < SCHEDULE_SIZE; index++) {
    char buffer[INTERNAL_CHAR_STR_SIZE_UNIT];
    snprintf(buffer, sizeof(buffer), "%d\n:", index);
    logger.printline(buffer);
    logger.printline(taskToCharStr(&schedule[index]));
  }
}
}
