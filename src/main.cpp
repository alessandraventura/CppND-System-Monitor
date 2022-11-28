#include "ncurses_display.h"
#include "system.h"

/*Alessandra*/
#include "linux_parser.h"
#include <iostream>

int main() {

  // std::cout << LinuxParser::OperatingSystem() << std::endl;
  // std::cout << LinuxParser::Kernel() << std::endl;
  // // std::cout << LinuxParser::Pids() << std::endl; // BONUS
  // std::cout << LinuxParser::MemoryUtilization() << std::endl;
  // std::cout << LinuxParser::UpTime() << std::endl;
  // // std::cout << LinuxParser::CpuUtilization() << std::endl;
  std::vector<std::string> ale = LinuxParser::CpuUtilization();
  System system; /* Alessandra : rimetti a posto il main */
  NCursesDisplay::Display(system);
}