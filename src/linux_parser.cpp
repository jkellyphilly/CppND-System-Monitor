#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// An example of how to read data from the filesystem
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

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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

// Read and return the system memory utilization
// Formula used is MemUsed = MemTotal - MemFree
float LinuxParser::MemoryUtilization() { 
  float utilization;
  string line;
  string key;
  string value;
  float mem_total = -1.0;
  float mem_free = -1.0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      line.erase(std::remove(line.begin(), line.end(), ':'), line.end());
      line.erase(std::remove(line.begin(), line.end(), '\t'), line.end());

      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          std::stringstream val(value);
          val >> mem_total;
        } else if (key == "MemFree") {
          std::stringstream val(value);
          val >> mem_free;
        }

        if (mem_total != -1.0 && mem_free != -1.0) {
          utilization = mem_total - mem_free;
          return (utilization / mem_total);
        }
      }
    } 
  }
  return 0.0;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime;
  string line, s_uptime, rest;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> s_uptime >> rest;
    uptime = std::stol(s_uptime);
    return uptime;
  }

  return 0; 
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  vector<string> cpu_util_vals = LinuxParser::CpuUtilization();
  long sum;
  for (string s : cpu_util_vals) {
    sum += std::stol(s);
  }

  return sum;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> total_jiffies = LinuxParser::CpuUtilization();
  long active_jiffies = std::stol(total_jiffies[0]) + std::stol(total_jiffies[1]) + std::stol(total_jiffies[2]) + std::stol(total_jiffies[5]) + std::stol(total_jiffies[6]) + std::stol(total_jiffies[7]);
  return active_jiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> total_jiffies = LinuxParser::CpuUtilization();
  // Only include idle and iowait!
  long idle_jiffies = std::stol(total_jiffies[3]) + std::stol(total_jiffies[4]);
  return idle_jiffies;
}

// Read and return CPU utilization values
vector<string> LinuxParser::CpuUtilization() { 
  string line;
  string key;
  string val1, val2, val3, val4, val5, val6, val7, val8, val9, val10;
  vector<string> cpu_util_vals;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> val1 >> val2 >> val3 >> val4 >> val5 >> val6 >> val7 >> val8 >> val9 >> val10) {
        if (key == "cpu") {
          return {val1, val2, val3, val4, val5, val6, val7, val8, val9, val10};
        }
      }
    }
  } 
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;
  int total_processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          std::stringstream val(value);
          val >> total_processes;
          return total_processes;
        }
      }
    }
  }
  return 0;
}

// TODO: consolidate this with TotalProcesses() above
// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  int running_processes;
  std::ifstream filestream(kProcDirectory + kStatFilename); 

  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          std::stringstream val(value);
          val >> running_processes;
          return running_processes;
        }
      }
    }
  }
  return 0;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmd;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, cmd)) {
      return cmd;
    }
  }
  return string(); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return value;
        }
      }
    }
  }
  return string(); 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { return 0; }
