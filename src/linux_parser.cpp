#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization (DONE) 
float LinuxParser::MemoryUtilization() {
  string key, kb;
  long value;
  string line;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  std::unordered_map<string, long> meminfo;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> value >> kb;
      meminfo[key] = value;
    }
    
    filestream.close();
    return (float)(meminfo["MemTotal"] - meminfo["MemFree"]) / meminfo["MemTotal"];
  }
}

// TODO: Read and return the system uptime (DONE) 
long LinuxParser::UpTime() { 
  string line;
  long uptime idle_time ;
  
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idle_time;
    filestream.close();
    
    return uptime;
  }
  return 0;
}

// TODO: Read and return the number of jiffies for the system (DONE) 
long LinuxParser::Jiffies() { return ActiveJiffies() + IdleJiffies();  }

// TODO: Read and return the number of active jiffies for a PID
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
// /proc/[PID]/stat # 14:utime, #15:stime, #16:cutime, #17:cstime, #22:starttime 
long LinuxParser::ActiveJiffies(int pid) {
  std::string line;
  
  long utime, stime, cutime, cstime, total_time;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> in_iter(linestream), eof;
    std::vector<string> single_word(in_iter, eof);
    
    utime = stol(single_word[13]);
    stime = stol(single_word[14]);
    cutime = stol(single_word[15]);
    cstime = stol(single_word[16]);
    
    total_time = utime + stime + cutime + cstime;
    filestream.close();
  }
  
  return total_time;
}

// TODO: Read and return the number of active jiffies for the system (DONE) 
long LinuxParser::ActiveJiffies() { 
  vector<string> cpu_jiffies = CpuUtilization();
  return stol(cpu_jiffies[kUser_]) + stol(cpu_jiffies[kNice_]) +
    stol(cpu_jiffies[kSystem_]) + stol(cpu_jiffies[kIRQ_]) +
    stol(cpu_jiffies[kSoftIRQ_]) + stol(cpu_jiffies[kSteal_]) +
    stol(cpu_jiffies[kGuest_]) + stol(cpu_jiffies[kGuestNice_]);
}

// TODO: Read and return the number of idle jiffies for the system (DONE) 
long LinuxParser::IdleJiffies() { 
  vector<string> cpu_jiffies = CpuUtilization();
  return stol(cpu_jiffies[kIdle_]) + stol(cpu_jiffies[kIOwait_]);
}

// TODO: Read and return CPU utilization (DONE) 
vector<string> LinuxParser::CpuUtilization() { 
  string line, cpu, value;
  vector<string> cpu_jiffies;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream  >> cpu;
    
    while (linestream >> value) {
      cpu_jiffiess.emplace_back(value);
    }
  }
  filestream.close();
  return cpu_jiffies;
}

// TODO: Read and return the total number of processes (DONE) 
int LinuxParser::TotalProcesses() { 
  string key;
  string line;
  int value; 

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes"){
        return value; 
      }
    }
    filestream.close();
  }
  return 0;
}

// TODO: Read and return the number of running processes (DONE) 
int LinuxParser::RunningProcesses() {
  string key;
  string line;
  int value;
  
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return value;
      }
    }
    filestream.close();
  }
  return 0;
}

// TODO: Read and return the command associated with a process (DONE) 
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string command;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, command);
  }
  filestream.close();
  return command;
}

// TODO: Read and return the memory used by a process (DONE) 
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line;
  string key, kb;
  int value;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value >> kb;
      if (key == "VmSize:") {
        return std::to_string(value / 1024);
      }
    }
    filestream.close();
  }
  return string("0");
}

// TODO: Read and return the user ID associated with a process (DONE) 
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  int value;
  
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") {
        return std::to_string(value);
      }
    }
    filestream.close();
  }
  
  return string("0");
}

// TODO: Read and return the user associated with a process (DONE)
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string line;
  string user, x, uid_for_user;
  
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      if (uid_for_user == Uid(pid)) {
        return user;
      }
    }
    filestream.close();
  }
  return string();
}

// TODO: Read and return the uptime of a process (DONE) 
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  long uptime_system = LinuxParser::UpTime();
  long uptime_process = 0, starttime;
  std::string line;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    std::istream_iterator<string> in_iter(linestream), eof;
    std::vector<string> single_word(in_iter, eof);
    starttime = stol(single_word[21]);
    
    uptime_process = uptime_system - (starttime / sysconf(_SC_CLK_TCK));
    filestream.close();
  }
  return uptime_process;
}
