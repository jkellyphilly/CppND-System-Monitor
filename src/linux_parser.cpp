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

// HELPER FUNCTIONS

template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  string line, key;
  T value;

  std::ifstream stream(filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  stream.close();
  return value;
}

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
  filestream.close();
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
  stream.close();
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
        if (key == filterMemTotalString) {
          std::stringstream val(value);
          val >> mem_total;
        } else if (key == filterMemFreeString) {
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
  filestream.close();
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
  filestream.close();
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
        if (key == filterCpu) {
          return {val1, val2, val3, val4, val5, val6, val7, val8, val9, val10};
        }
      }
    }
  } 
  filestream.close();
  return cpu_util_vals;
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
        if (key == filterProcesses) {
          std::stringstream val(value);
          val >> total_processes;
          return total_processes;
        }
      }
    }
  }
  filestream.close();
  return 0;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  return findValueByKey<int>(filterRunningProcesses, (kProcDirectory + kStatFilename));
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
  filestream.close();
  return string(); 
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  return findValueByKey<string>(filterProcMem, (kProcDirectory + to_string(pid) + kStatusFilename));
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  return findValueByKey<string>(filterUID, (kProcDirectory + to_string(pid) + kStatusFilename));
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = LinuxParser::Uid(pid);
  string value;
  string line;
  string user;
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> value) {
        
        size_t end = value.find(':');
        size_t start = 0;
        user = value.substr(start, end);
        
        for (int i=0; i<2; i++) {
          start = end + 1; // 1 to account for the colon itself
          end = value.find(':', start);
        }

        // we might need to grab 2 characters if UID is double digits, 
        // thus take N chars where N is the diff b/w the current end
        // and start pos values
        size_t len_of_segment = end - start;
        string uid_from_file = value.substr(start, len_of_segment);

        if (uid_from_file == uid) {
          return user;
        }
      }
    }
  }
  filestream.close();
  return string();
}

vector<long> LinuxParser::StatValues(int pid) {
  string line;
  string value;
  vector<long> values = {};

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    size_t start = 0;
    size_t end = line.find(' ', start);
    while (end != std::string::npos || end < 23) {
      value = line.substr(start, end);
      long val;
      try 
      {
        val = std::stol(value);
      } 
      catch (const std::exception& e)
      {
        val = 0;
      }

      values.emplace_back(val);
      start = end + 1; // 1 to account for the space itself
      end = line.find(' ', start);
    }
  }
  filestream.close();
  return values;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  vector<long> stat_values = LinuxParser::StatValues(pid);

  long uptime = stat_values[21]/sysconf(_SC_CLK_TCK);
  return uptime;
}
