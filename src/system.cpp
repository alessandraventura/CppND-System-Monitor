#include <cstddef>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"
#include <iostream>

// Return the system's CPU
Processor &System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
std::vector<Process> &System::Processes() {
  auto pids = LinuxParser::Pids();
  processes_.clear();
  for (auto &i : pids) {
    processes_.emplace_back(Process(i));
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }
