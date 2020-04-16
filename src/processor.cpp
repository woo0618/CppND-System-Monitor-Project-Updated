#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
  const long prev_idle = LinuxParser::IdleJiffies();
  const long pre_total = LinuxParser::Jiffies();
  
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  long idle = LinuxParser::IdleJiffies();
  long total = LinuxParser::Jiffies();
  
  long totald = total - pre_total;
  long idled = idle - prev_idle;
  return (float)(totald - idled) / totald;
}
