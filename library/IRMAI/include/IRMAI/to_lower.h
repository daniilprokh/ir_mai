#ifndef TO_LOWER_H_IR_MAI
#define TO_LOWER_H_IR_MAI

#include <string>

inline void ToLower(std::string& str) {
  for (char& c : str) {
    if (c < 'A' || c > 'Z') {
      continue;
    }

    c += 32;
  }
}

#endif
