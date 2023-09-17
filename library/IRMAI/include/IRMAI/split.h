#pragma once

#include <string>
#include <string_view>
#include <vector>

inline std::vector<std::string> Split(std::string_view str, char separator = ' ') {
  std::vector<std::string> res;
  while (true) {
    size_t pos = str.find(separator);
    res.emplace_back(str.substr(0, pos));
    
    if (pos == str.npos) {
        break;
    }

    str.remove_prefix(pos + 1);
  }
  return res;
}
