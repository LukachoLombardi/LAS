#pragma once

struct LASConfig {
  int scheduleSize = 32;
  bool lagWarning = false;
  int maxLagMs = 50;
  int strSize = 128;
};
