#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <sstream>
#include <vector>
#include <iterator>

using namespace std;

class Utils {
public:
    static string lowerString(string str);
    
    template<typename Out>
    static void split(const string &s, char delim, Out result);

    static vector<string> split(const string &s, char delim);
    static vector<string> resplit(const string & s, string rgx_str = "\\s+");
};

#endif
