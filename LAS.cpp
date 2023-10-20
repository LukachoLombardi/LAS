#include <Arduino.h>
#include "LAS.h"
#include <Arduino.h>
#include "LASConfig.h"
#include <Logger.h>

namespace LAS {
Task schedule[SCHEDULE_SIZE] = {Task{
    false,
    NULL,
    0,
    false,
    0}
};

int activeTaskIndex = 0;

int getActiveTaskIndex(){
  return activeTaskIndex;
}
Task getActiveTask(){
  return getTask(activeTaskIndex);
}

int determineFirstInactiveIndex(Task array[], int length) {
  for (int index = 0; index < length; index++) {
    if (!array[index].isActive) {
      return index;
    }
  }
  return length;
}

void scheduleFunction(void (*func)(), long triggerTime, bool repeat, int repeatInterval, int remainingRepeats) {
  schedule[determineFirstInactiveIndex(schedule, SCHEDULE_SIZE)] = Task{
    true,
    func,
    triggerTime,
    repeat,
    repeatInterval,
    remainingRepeats
  };
}

void scheduleIn(void (*func)(), long triggerDelay) {
  scheduleFunction(func, millis() + triggerDelay);
}

void scheduleRepeated(void (*func)(), int repeatInterval, int repeats) {
  scheduleFunction(func, millis() + repeatInterval, true, repeatInterval, repeats);
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
  Serial.println("Lukacho's Amazing Scheduler - alpha - now with repeats!");
  Serial.println();
}
void runScheduler() {
  Serial.println("starting scheduler...");
  printWelcome();
  if (schedule[0].isActive) {
    Serial.println("WARNING: Please consider adding initial Tasks through an ASAP Task for high precision apps");
  }
  if (activeTaskIndex != 0) {
    Serial.println("FATAL: ACTIVETASKINDEX != 0. THERE MIGHT BE ANOTHER SCHEDULER ALREADY RUNNING!");
  }
  while (true) {
    for (int index = 0; index < SCHEDULE_SIZE; index++) {
      activeTaskIndex = index;
      Task currentTask = schedule[index];
      if ((currentTask.isActive) &&
          (currentTask.func != NULL) &&
          (currentTask.triggerTime <= millis())) {

        if (millis() - currentTask.triggerTime >= CRITICAL_LAG_MS && currentTask.triggerTime != 0) {
          Serial.println("WARNING: SCHEDULER IS FALLING BEHIND CRITICALLY!");
        }

        currentTask.func();
        if (currentTask.repeat) {
          schedule[index].triggerTime = millis() + currentTask.repeatInterval;
        } else {
          schedule[index].isActive = false;
        }

        if (currentTask.remainingRepeats != ENDLESS_LOOP) {
          schedule[index].remainingRepeats--;
        }

        if (schedule[index].remainingRepeats == 0) {
          schedule[index].isActive = false;
        }
      }
    }
  }
}

char* taskToCharStr(Task task) {
  static char buffer[INTERNAL_CHAR_STR_SIZE_UNIT];
  snprintf(buffer, sizeof(buffer), "Task:\n  isActive: %d\n  func: %p\n  triggerTime: %d\n  repeat: %d\n  repeatInterval: %d",
           task.isActive, task.func, task.triggerTime, task.repeat, task.repeatInterval);
  return buffer;
}
char* scheduleToCharStr() {
  static char buffer[INTERNAL_CHAR_STR_SIZE_UNIT * SCHEDULE_SIZE];
  for (int index = 0; index < SCHEDULE_SIZE; index++) {
    static char indexBuffer[3];
    snprintf(indexBuffer, sizeof(buffer), "\n%d:\n", index);
    strcat(buffer, indexBuffer);
    strcat(buffer, taskToCharStr(schedule[index]));
  }
  return buffer;
}

Task getTask(int index) {
  return schedule[index];
}

void printSchedule() {
  for (int index = 0; index < SCHEDULE_SIZE; index++) {
    Serial.print(index); Serial.print(":\n");
    Serial.println(taskToCharStr(schedule[index]));
  }
}
}
