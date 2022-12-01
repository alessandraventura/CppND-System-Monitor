#include "processor.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
  float active_jiffies = float(LinuxParser::ActiveJiffies());
  float jiffies = float(LinuxParser::Jiffies());
  if (jiffies != 0) {
    return (active_jiffies / jiffies);
  } else
    return 0;
}