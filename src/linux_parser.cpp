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
  float value, mem_total, mem_free, mem_available;
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
  std::ifstream filestream(kProcDirectory + kStatFilename);
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
  long totaltime;
  long utime = 0, stime = 0, cutime = 0, cstime = 0;
  std::string line, value;
  std::vector<std::string> values;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      values.push_back(value);
    }
    if (std::all_of(values[13].begin(), values[13].end(), isdigit))
      utime = std::stol(values[13]);
    if (std::all_of(values[14].begin(), values[14].end(), isdigit))
      stime = std::stol(values[14]);
    if (std::all_of(values[15].begin(), values[15].end(), isdigit))
      cutime = std::stol(values[15]);
    if (std::all_of(values[16].begin(), values[16].end(), isdigit))
      cstime = std::stol(values[16]);
  }
  totaltime = utime + stime + cutime + cstime;
  return totaltime / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  long total_jiffies = 0;

  if (CPUStates::kUser_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kUser_).begin(),
                  jiffies.at(CPUStates::kUser_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kUser_));
  }
  if (CPUStates::kNice_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kNice_).begin(),
                  jiffies.at(CPUStates::kNice_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kNice_));
  }
  if (CPUStates::kSystem_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kSystem_).begin(),
                  jiffies.at(CPUStates::kSystem_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kSystem_));
  }
  if (CPUStates::kIRQ_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kIRQ_).begin(),
                  jiffies.at(CPUStates::kIRQ_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kIRQ_));
  }
  if (CPUStates::kSoftIRQ_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kSoftIRQ_).begin(),
                  jiffies.at(CPUStates::kSoftIRQ_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kSoftIRQ_));
  }
  if (CPUStates::kSteal_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kSteal_).begin(),
                  jiffies.at(CPUStates::kSteal_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kSteal_));
  }

  return total_jiffies / sysconf(_SC_CLK_TCK);
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  std::vector<std::string> jiffies = CpuUtilization();
  long total_jiffies = 0;

  if (CPUStates::kIdle_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kIdle_).begin(),
                  jiffies.at(CPUStates::kIdle_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kIdle_));
  }
  if (CPUStates::kIOwait_ < jiffies.size() &&
      std::all_of(jiffies.at(CPUStates::kIOwait_).begin(),
                  jiffies.at(CPUStates::kIOwait_).end(), isdigit)) {
    total_jiffies += stol(jiffies.at(CPUStates::kIOwait_));
  }

  return total_jiffies / sysconf(_SC_CLK_TCK);
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
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> username >> value >> key) {
        if (key == Uid(pid)) {
          return username;
        }
      }
    }
  }
  return std::string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  std::string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 0; i < 22; i++) {
        linestream >> value;
      }
    }
  }
  return stol(value) / sysconf(_SC_CLK_TCK);
}
