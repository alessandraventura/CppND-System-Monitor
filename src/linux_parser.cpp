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

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies(); }

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  std::vector<long> key;
  long active_jiffies = 0, value = 0;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        key.emplace_back(value);
      }
    }

    for (int i = 13; i < 17; i++) {
      active_jiffies += key.at(i);
    }
  }
  return active_jiffies;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> active_jiffies = CpuUtilization();

  return 0; // stol(active_jiffies.at(CPUStates::kUser_)) +
            // stol(active_jiffies.at(CPUStates::kNice_)) +
            // stol(active_jiffies.at(CPUStates::kSystem_)) +
            // stol(active_jiffies.at(CPUStates::kIRQ_)) +
            // stol(active_jiffies.at(CPUStates::kSoftIRQ_)) +
            // stol(active_jiffies.at(CPUStates::kSteal_));
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> active_jiffies = CpuUtilization();

  return 0; // stol(active_jiffies.at(CPUStates::kIdle_)) +
            // stol(active_jiffies.at(CPUStates::kIOwait_));
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  std::string line;
  std::string key;
  int value = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
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
  long value;
  std::vector<long> key(22);
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        key.emplace_back(value);
      }
    }
  }

  return key.at(21);
}
