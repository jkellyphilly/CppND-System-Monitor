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
        // I can just push back an int. This calls the constructor 
        // for the Process class with an int (pid) and returns
        // a process object. Cool!
        // It works because my processes_ vector is defined as 
        // having the type of Process for item in it. So when we push
        // new into the vector, Process constructor is called. Woah.
        processes_.push_back(pid);
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
