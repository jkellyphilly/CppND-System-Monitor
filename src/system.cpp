#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using namespace std;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    const vector<int> & pids = LinuxParser::Pids();
    for (const int pid : pids) {
        Process* proc = new Process(pid);
        // Only push back a process if it has a CMD, user, and 
        // RAM associated with it, as well as a non-zero CPU 
        // utilization. 
        if (!(*proc).Command().empty() 
            && !(*proc).User().empty()
            && !(*proc).Ram().empty()
            && ((*proc).CpuUtilization() != 0.0)) {
            processes_.push_back(*proc);
        }
    }
    return processes_; 
}

// Return the system's kernel identifier (string) by calling
// the LinuxParser method
string System::Kernel() { 
    return LinuxParser::Kernel(); 
}

// Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization(); 
}

// Return OS name by calling LinuxParser method
string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem();
}

// Return the number of processes actively running on the system
int System::RunningProcesses() { 
    return LinuxParser::RunningProcesses(); 
}

// Return the total number of processes on the system
int System::TotalProcesses() { 
    return LinuxParser::TotalProcesses(); 
}

// Return the number of seconds since the system started running
long int System::UpTime() { 
    return LinuxParser::UpTime(); 
}
