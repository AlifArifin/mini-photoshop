#include "utils.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <regex>

using namespace std;

string Utils::lowerString(string str) {
    for_each(str.begin(), str.end(), [](char & c) {
        c = tolower(c);
    });

    return str;
}

template<typename Out>
void Utils::split(const string &s, char delim, Out result) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        *(result++) = item;
    }
}

vector<string> Utils::split(const string &s, char delim) {
    vector<string> elems;

    stringstream ss(s);
    string item;
    while (std::getline(ss, item, delim)) {
        if (item.length() > 0) {
            elems.push_back(item);
        }
    }
    return elems;
}

vector<string> Utils::resplit(const string & s, string rgx_str) {
    vector<string> elems;

    regex rgx (rgx_str);

    sregex_token_iterator iter(s.begin(), s.end(), rgx, -1);
    sregex_token_iterator end;

    while (iter != end)  {
      //cout << "S43:" << *iter << endl;
      elems.push_back(*iter);
      ++iter;
    }

    return elems;
}
