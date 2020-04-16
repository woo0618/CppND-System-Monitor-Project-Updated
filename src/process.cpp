#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : _pid (pid) {
  _cmd = LinuxParser::Command(_pid);
  _ram = LinuxParser::Ram(_pid);
  _user = LinuxParser::User(_pid);
  _uptime = LinuxParser::UpTime(_pid);
  _cpu_usage = ((float)LinuxParser::ActiveJiffies(_pid) / sysconf(_SC_CLK_TCK)) / UpTime();
}

// TODO: Return this process's ID (DONE)
int Process::Pid() { return _pid; }

// TODO: Return this process's CPU utilization (DONE)
// cpu_usage = 100 * ((ActiveJiffies(pid) / Hertz) / Uptime(pid))
float Process::CpuUtilization() { return _cpu_usage; }

// TODO: Return the command that generated this process (DONE) 
string Process::Command() { return _cmd; }

// TODO: Return this process's memory utilization (DONE) 
string Process::Ram() { return _ram; }

// TODO: Return the user (name) that generated this process (DONE)
string Process::User() { return _user; }

// TODO: Return the age of this process (in seconds) (DONE) 
long int Process::UpTime() { return _uptime; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {return stoi(this->_ram) < stoi(a._ram);}
