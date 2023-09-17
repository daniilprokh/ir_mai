#ifndef FILE_SIZE_CONVERTER_H_IR_MAI
#define FILE_SIZE_CONVERTER_H_IR_MAI

#include <cmath>

struct FileSizeConverter {
  public:
    enum class BinaryPrefix {
      kMissing,
      kK,
      kM,
      kG
    };

    static double Convert(double size, BinaryPrefix from, BinaryPrefix to) {
      int difference = static_cast<int>(from) - static_cast<int>(to);
      return size * std::pow(1024, difference);
    }
};

#endif