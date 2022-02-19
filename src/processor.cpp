#include "processor.h"
#include <string>
#include <vector>
#include "linux_parser.h"

using std::string;
using std::vector;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    vector<string> cpu_util_vals = LinuxParser::CpuUtilization();
    return std::stof(cpu_util_vals[0]); 
}