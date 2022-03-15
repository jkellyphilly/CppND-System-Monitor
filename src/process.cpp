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

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    vector<long> stat_vals = LinuxParser::StatValues(Process::Pid());
    long system_uptime = LinuxParser::UpTime();
    long time_since_process_started = system_uptime - LinuxParser::UpTime(Process::Pid());

    // To calculate values, I used this StackOverflow post:
    // https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    
    // Get the total time spent for this process
    long total_time_process = stat_vals[13] + stat_vals[14] + stat_vals[15] + stat_vals[16];
    // convert to seconds
    long total_time_process_seconds = total_time_process/sysconf(_SC_CLK_TCK);

    return (float)total_time_process_seconds/(float)time_since_process_started; 
}

// Return the command that generated this process
string Process::Command() { 
    return LinuxParser::Command(Process::Pid());
}

// Return this process's memory utilization.
// Note that I've only include 7 sig digits for 
// formatting purposes.
string Process::Ram() { 
    string ram_kb = LinuxParser::Ram(Process::Pid()); 
    float ram_kb_float = std::stof(ram_kb);
    float ram_mb_float = ram_kb_float/1000;
    return std::to_string(ram_mb_float).substr(0, 7);
}

// Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(Process::Pid()); 
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(Process::Pid()); 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    string this_ram_kb = LinuxParser::Ram(pid_);
    string a_ram_kb = LinuxParser::Ram(a.pid_);
    return std::stof(this_ram_kb) < std::stof(a_ram_kb); 
}