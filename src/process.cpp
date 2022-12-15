#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const {
  float active_jiffies = float(LinuxParser::ActiveJiffies(pid_));
  float jiffies = float(LinuxParser::UpTime() - LinuxParser::UpTime(pid_));
  if (jiffies != 0) {
    return (active_jiffies / jiffies);
  }
  return 0;
}

// Return the command that generated this process
std::string Process::Command() {
  std::string command = LinuxParser::Command(pid_);

  if (command.size() > 40) {
    command.erase(command.begin() + 40, command.end());
    command += "...";
  }

  return command;
}

// Return this process's memory utilization
std::string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
std::string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() {
  return LinuxParser::UpTime() - LinuxParser::UpTime(pid_);
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const &a) const {
  return a.CpuUtilization() < CpuUtilization();
}