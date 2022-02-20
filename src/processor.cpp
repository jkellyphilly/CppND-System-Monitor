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
    vector<string> cpu_util_vals = LinuxParser::CpuUtilization();
    float user = std::stof(cpu_util_vals[0]);
    float nice = std::stof(cpu_util_vals[1]);
    float system = std::stof(cpu_util_vals[2]);
    float idle = std::stof(cpu_util_vals[3]);
    float iowait = std::stof(cpu_util_vals[4]);
    float irq = std::stof(cpu_util_vals[5]);
    float softirq = std::stof(cpu_util_vals[6]);
    float steal = std::stof(cpu_util_vals[7]);
    float guest = std::stof(cpu_util_vals[8]);
    float guest_nice = std::stof(cpu_util_vals[9]);

    float total_idle = idle + iowait;
    float total_non_idle = user + nice + system + irq + softirq + steal;
    return total_non_idle/(total_idle + total_non_idle);
}