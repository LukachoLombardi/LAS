#include "LAS.h"
#include <Arduino.h>

namespace LAS {
    Task schedule[SCHEDULE_SIZE] = {Task{
        false,
        NULL,
        0,
        false,
        0}
    };

    int determineFirstInactiveIndex(Task array[], int length) {
        for (int index = 0; index < length; index++) {
            if (!array[index].isActive) {
                return index;
            }
        }
        return length;
    }

    void scheduleAt(void (*func)(), long triggerTime, bool repeat, int repeatInterval) {
        schedule[determineFirstInactiveIndex(schedule, SCHEDULE_SIZE)] = Task{
            true,
            func,
            triggerTime,
            repeat,
            repeatInterval
        };
    }

    void scheduleIn(void (*func)(), long triggerDelay) {
        scheduleAt(func, millis() + triggerDelay);
    }

    void scheduleRepeated(void (*func)(), int repeatInterval){
      scheduleAt(func, millis() + repeatInterval, true, repeatInterval);
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
        Serial.println("Lukacho's Amazing Scheduler - alpha v0.2.0 - now with repeats!");
    }

    void runScheduler() {
        Serial.begin(FALLBACK_BAUDRATE);
        Serial.println("starting scheduler");
        printWelcome();
        delay(500);
        while (true) {
            for (int index = 0; index < SCHEDULE_SIZE; index++) {
                Task currentTask = schedule[index];
                if ((currentTask.isActive) &&
                  (currentTask.func != NULL) && 
                (currentTask.triggerTime <= millis())) {
                  
                    currentTask.func();
                    if(currentTask.repeat){
                      schedule[index].triggerTime += currentTask.repeatInterval;
                    } else {
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
    char* scheduleToCharStr(){
      static char buffer[INTERNAL_CHAR_STR_SIZE_UNIT*SCHEDULE_SIZE];
      for(int index=0;index<SCHEDULE_SIZE;index++){
        static char indexBuffer[3];
        snprintf(indexBuffer, sizeof(buffer), "\n%d:\n", index);
        strcat(buffer, indexBuffer);
        strcat(buffer, taskToCharStr(schedule[index]));
      }
      return buffer;
    }

    Task getTask(int index){
      return schedule[index];
    }
}
