#include "format.h"

#include <unistd.h>

#include <string>

std::string Format::TwoDigits(int n) {
  if (n < 10) {
    return "0" + std::to_string(n);
  } else {
    return std::to_string(n);
  }
}

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long seconds) {
  int hours = 0;
  int mins = 0;
  std::string string_seconds, string_minutes, string_hours;

  mins = seconds / 60;
  seconds = seconds - (mins * 60);
  string_seconds = TwoDigits(seconds);
  hours = mins / 60;
  mins = mins - (hours * 60);
  string_minutes = TwoDigits(mins);
  string_hours = TwoDigits(hours);

  return string_hours + ":" + string_minutes + ":" + string_seconds;
}