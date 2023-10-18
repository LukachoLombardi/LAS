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
        int remaingRepeats;
    };

    extern Task schedule[SCHEDULE_SIZE];

    int determineFirstFreeIndex(Task array[], int length);
    void scheduleFunction(void (*func)(), long triggerTime = ASAP, bool repeat = false, int repeatIntervall = 0, int remaingRepeats = -1);
    void scheduleIn(void (*func)(), long triggerDelay);
    void scheduleRepeated(void (*func)(), int repeatInterval = ASAP, int remaingRepeats = -1);
    void printWelcome();
    Task getTask(int index);
    char* taskToCharStr(Task task);
    char* scheduleToCharStr(); //WARNING: VERY MEMORY HUNGRY, WILL PROBABLY CRASH YOUR ARDUINO
    void printSchedule();
    void runScheduler();
}
