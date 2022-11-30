#include <cctype>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::to_string;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() {
  return LinuxParser::ActiveJiffies(pid_) / LinuxParser::Jiffies();
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

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const &a [[maybe_unused]]) const {
  return true;
}