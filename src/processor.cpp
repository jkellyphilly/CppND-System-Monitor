#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::vector;

// Return the aggregate CPU utilization
// I calculated this (without the "prev" versioning) as defined in this SO post:
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() { 
    float active = (float) LinuxParser::ActiveJiffies();
    float idle = (float) LinuxParser::IdleJiffies();

    return active / (active + idle);
}