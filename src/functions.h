#ifndef PLASZCZAKI_GIT_CHLOPAKI_FUNCTIONS_H
#define PLASZCZAKI_GIT_CHLOPAKI_FUNCTIONS_H

#include <algorithm>
#include <set>
#include <string>
#include <unordered_map>

std::string trim(const std::string& str);

std::string usunInterpunkcje(const std::string& tekst);

std::unordered_map<char, std::set<char>> utworzMapeOdwracen();

#endif  // PLASZCZAKI_GIT_CHLOPAKI_FUNCTIONS_H
