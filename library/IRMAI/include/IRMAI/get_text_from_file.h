#ifndef GET_TEXT_FROM_FILE_H_IR_MAI
#define GET_TEXT_FROM_FILE_H_IR_MAI

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include <iostream>

inline std::string GetTextFromFile(const std::filesystem::path& filePath) {
  std::ifstream input(filePath);
  if (!input.is_open()) {
    return std::string();
  }

  std::stringstream buffer;
  buffer << input.rdbuf();
  return buffer.str();
}

#endif
