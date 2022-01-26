#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    string h = std::to_string(seconds / 3600); 
    string hh = string(2 - h.length(), '0') + h;
    string m = std::to_string((seconds % 3600) / 60);
    string mm = string(2 - m.length(), '0') + m;
    string s = std::to_string((seconds % 3600) % 60);
    string ss = string(2 - s.length(), '0') + s;
    string result = hh + ":" + mm + ":" + ss;
    return result; 
}