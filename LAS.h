/*#pragma once

namespace LAS {
  #define SCHEDULE_SIZE 32
  #define FALLBACK_BAUDRATE 9600

  struct Task {
    void (*func)();
    long triggerTime;
    bool triggerASAP;
  };

  int determineFirstFreeIndex(Task array[], int length){
    for(int index=0;index<length;index++){
      if(array[index].func == NULL){
        return index;
      }
    }
    return length;
  }

  Task schedule[SCHEDULE_SIZE] = {Task{
    NULL,
    0,
    false}
  };

  void scheduleAt(void (*func)(), bool triggerASAP, long triggerTime = 0) {
    schedule[determineFirstFreeIndex(schedule, SCHEDULE_SIZE)] = Task
  {
      func,
      triggerTime,
      triggerASAP
    };
  }

  void scheduleIn(void (*func)(), long triggerDelay){
    scheduleAt(func, false, millis()+triggerDelay);
  }

  void printWelcome(){
  Serial.println("This Unit is running");
  Serial.println(" __       ______  ____       ");
  Serial.println("/\\ \\     /\\  _  \\/\\  _`\\     ");
  Serial.println("\\ \\ \\    \\ \\ \\L\\ \\ \\,\\L\\_\\   ");
  Serial.println(" \\ \\ \\  __\\ \\  __ \\/_\\__ \\   ");
  Serial.println("  \\ \\ \\L\\ \\\\ \\ \\/\\ \\/\\ \\L\\ \\ ");
  Serial.println("   \\ \\____/ \\ \\_\\ \\_\\ `\\____\\\\");
  Serial.println("    \\/___/   \\/_/\\/_/\\/_____/");
  Serial.println("");
  Serial.println("Lukacho's Amazing Scheduler - alpha v0.1.0");
  }

  void runScheduler(){
    Serial.begin(FALLBACK_BAUDRATE);
    Serial.println("starting scheduler");
    printWelcome();
    delay(500);
    while(true){
      for(int index=0;index<determineFirstFreeIndex(schedule, SCHEDULE_SIZE);index++){
      if((schedule[index].triggerTime <= millis() || schedule[index].triggerASAP == tr,ue)&&
      (schedule[index].func != NULL)){
        Task currentTask = schedule[index];
        schedule[index] = Task{
          NULL,
          0,
          false
        };
        currentTask.func();
        }
      }
    }
  }
}*/

#pragma once

namespace LAS {
    #include "LASConfig.h"

    struct Task {
        bool isActive; //whether or not a Task is actively maintained. inactive tasks are invalid and can be deleted or overwritten at any time.
        void (*func)(); //function pointer to the scheduled function
        long triggerTime; //time in millis to next trigger func
        //bool triggerASAP; //if true: ignores triggerTime and gets triggered on first discovery in the queue. DISCONTINUED: USE triggerTime = 0
        bool repeat; //if true: adds repeatIntervall to triggerTime instead of deactivating task right before execution, creating a recurring task.
        int repeatInterval;
    };

    extern Task schedule[SCHEDULE_SIZE];

    int determineFirstFreeIndex(Task array[], int length);
    void scheduleAt(void (*func)(), long triggerTime = ASAP, bool repeat = false, int repeatIntervall = 0);
    void scheduleIn(void (*func)(), long triggerDelay);
    void scheduleRepeated(void (*func)(), int repeatInterval);
    void printWelcome();
    Task getTask(int index);
    char* taskToCharStr(Task task);
    char* scheduleToCharStr(); //WARNING: VERY MEMORY HUNGRY, WILL PROBABLY CRASH YOUR ARDUINO
    void runScheduler();
}
