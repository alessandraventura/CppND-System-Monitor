#include <string>
#include <unistd.h>

#include "format.h"

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long seconds) {

  int hours = 0;
  int mins = 0;

  mins = seconds / 60;
  seconds = seconds - (mins * 60);
  hours = mins / 60;
  mins = mins - (hours * 60);

  return std::to_string(hours) + ":" + std::to_string(mins) + ":" +
         std::to_string(seconds);
}