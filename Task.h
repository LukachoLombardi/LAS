#pragma once

// forward declare for cyclical import
class Callable;

struct Task {
  bool isActive;  //whether or not a Task is actively maintained. inactive tasks are invalid and can be deleted or overwritten at any time.
  Callable* callable;
  bool deleteAfter;
  long triggerTime;  //time in millis to next trigger func
  bool repeat;       //if true: adds repeatIntervall to triggerTime instead of deactivating task right before execution, creating a recurring task.
  int repeatInterval;
  int remainingRepeats;
};
