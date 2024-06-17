#pragma once

struct LASConfig {
  int scheduleSize = 16;
  bool lagWarning = false;
  int maxLagMs = 50;
  int strSize = 128;

  LASConfig(int scheduleSize = 16, bool lagWarning = false, int maxLagMs = 50, int strSize = 128): 
  scheduleSize(scheduleSize), lagWarning(lagWarning), maxLagMs(maxLagMs), strSize(strSize) {}
};
