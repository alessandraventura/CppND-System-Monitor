#include <dirent.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include "linux_parser.h"

#include <iostream>

// Reading the Operating System name from the filesystem
std::string LinuxParser::OperatingSystem() {
  std::string line;
  std::string key;
  std::string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// Reading Kernel version from the filesystem
std::string LinuxParser::Kernel() {
  std::string os, kernel, version;
  std::string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
std::vector<int> LinuxParser::Pids() {
  std::vector<int> pids;
  DIR *directory = opendir(kProcDirectory.c_str());
  struct dirent *file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      std::string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  std::string line;
  std::string key;
  float value, mem_total, mem_free, mem_available, mem_used;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          mem_total = value;
        } else if (key == "MemFree:") {
          mem_free = value;
        } else if (key == "MemAvailable:") {
          mem_available = value;
        }
      }
    }
  }
  return (mem_total - mem_free) / mem_available;
}

// Read and return the system uptime (rounded)
long LinuxParser::UpTime() {
  long uptime;
  std::string line;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> uptime;
    }
  }
  return uptime;
}

// Read and return CPU utilization
std::vector<std::string> LinuxParser::CpuUtilization() {
  std::vector<std::string> jiffies;
  std::string line, cpu, value;
  std::ifstream filestream(kProcDirectory + kCpuinfoFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    linestream >> cpu;

    while (linestream >> value) {
      jiffies.emplace_back(value);
    }
  }
  return jiffies;
}

// QUESTION: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

// QUESTION: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  std::vector<std::string> key;
  long value = 0;
  std::ifstream filestream(
      kProcDirectory + std::to_string(pid) +
      kStatFilename); // TODO: a single function to operate with this
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key.at(0) >> key.at(1) >> key.at(2) >> key.at(3) >>
          key.at(4) >> key.at(5) >> key.at(6) >> key.at(7) >> key.at(8) >>
          key.at(9) >> key.at(10) >> key.at(11) >> key.at(12) >> key.at(13) >>
          key.at(14) >> key.at(15) >> key.at(16);
    }

    for (int i = 13; i < 17; i++) {
      value += stol(key.at(i));
    }
  }
  return value;
}

// QUESTION: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> cpu_jiffies(10);
  std::string cpu_name;
  std::string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  long active_jiffies = 0;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu_name >> cpu_jiffies.at(0) >> cpu_jiffies.at(1) >>
          cpu_jiffies.at(2) >> cpu_jiffies.at(3) >> cpu_jiffies.at(4) >>
          cpu_jiffies.at(5) >> cpu_jiffies.at(6) >> cpu_jiffies.at(7) >>
          cpu_jiffies.at(8) >> cpu_jiffies.at(9);
    }

    for (auto &jiff : cpu_jiffies) {
      active_jiffies += stol(jiff);
    }
  }

  return active_jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> cpu_jiffies(
      10); // TODO: create a single function to read the jiffies
  std::string cpu_name;
  std::string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> cpu_name >> cpu_jiffies.at(0) >> cpu_jiffies.at(1) >>
          cpu_jiffies.at(2) >> cpu_jiffies.at(3) >> cpu_jiffies.at(4) >>
          cpu_jiffies.at(5) >> cpu_jiffies.at(6) >> cpu_jiffies.at(7) >>
          cpu_jiffies.at(8) >> cpu_jiffies.at(9);
    }
  }

  return stol(cpu_jiffies.at(3)) + stol(cpu_jiffies.at(4));
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line;
  std::string key;
  int value = 0;
  std::ifstream filestream(
      kProcDirectory +
      kMeminfoFilename); // TODO: a single function to operate with this
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line;
  std::string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
std::string LinuxParser::Command(int pid) {
  std::string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
  }
  return line;
}

// Read and return the memory used by a process
std::string LinuxParser::Ram(int pid) {
  std::string line, key, value, unit;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "VmSize:") {
          return std::to_string(stol(value) / 1024);
        }
      }
    }
  }
  return value;
}

// Read and return the user ID associated with a process
std::string LinuxParser::Uid(int pid) {
  std::string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the user associated with a process
std::string LinuxParser::User(int pid) {
  std::string line, username, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> username >> value >> key) {
        if (key == Uid(pid)) {
          return username;
        }
      }
    }
  }
  return username;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line;
  std::vector<std::string> key(22);
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key.at(0) >> key.at(1) >> key.at(2) >> key.at(3) >>
          key.at(4) >> key.at(5) >> key.at(6) >> key.at(7) >> key.at(8) >>
          key.at(9) >> key.at(10) >> key.at(11) >> key.at(12) >> key.at(13) >>
          key.at(14) >> key.at(15) >> key.at(16) >> key.at(17) >> key.at(18) >>
          key.at(19) >> key.at(20) >> key.at(21);
    }
  }

  return stol(key.at(21));
}
