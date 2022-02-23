#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() { return 0; }

// Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(Process::Pid());
}

// Return this process's memory utilization
string Process::Ram() { 
    string ram_kb = LinuxParser::Ram(Process::Pid()); 
    float ram_kb_float = std::stof(ram_kb);
    float ram_mb_float = ram_kb_float/1000;
    return std::to_string(ram_mb_float).substr(0, 7);
}

// TODO: Return the user (name) that generated this process
string Process::User() { return string(); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return 0; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { return true; }