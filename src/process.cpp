#include <cctype>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"
#include "process.h"

// Return this process' ID
int Process::Pid() {
  return 0; // pid_;
}

// Set this process' ID
void Process::Pid(int pid) { pid_ = pid; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  return 0; // LinuxParser::ActiveJiffies(pid_) / LinuxParser::Jiffies();
}

// Return the command that generated this process
std::string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
std::string Process::Ram() { return LinuxParser::Ram(pid_); }

// Return the user (name) that generated this process
std::string Process::User() {
  user_ = LinuxParser::User(pid_);
  return user_;
}

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const &a [[maybe_unused]]) const {

  return false; //(a.pid_ < this->pid_) ? true : false;
}