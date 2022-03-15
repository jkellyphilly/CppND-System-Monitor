#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::vector;

// Return the aggregate CPU utilization
float Processor::Utilization() { 
    float active = (float) LinuxParser::ActiveJiffies();
    float idle = (float) LinuxParser::IdleJiffies();

    return active / (active + idle);
}