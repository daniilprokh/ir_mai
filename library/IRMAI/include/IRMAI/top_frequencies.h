#ifndef TOP_FREQUENCIES_H_IR_MAI
#define TOP_FREQUENCIES_H_IR_MAI

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

struct TokenInfo {
  TokenInfo() = default;

  TokenInfo(const TokenInfo&) = default;
  TokenInfo& operator=(const TokenInfo&) = default;

  TokenInfo(TokenInfo&&) = default;
  TokenInfo& operator=(TokenInfo&&) = default;

  TokenInfo(const std::string &str, unsigned long long frequencies)
    : str_(str), frequencies_(frequencies)
  {}

  std::string str_;
  unsigned long long frequencies_;
};

inline std::vector<TokenInfo> TopFrequencies(
    const std::unordered_map<std::string, unsigned long long> &tokenFrequencies,
    int count) {
  std::vector<TokenInfo> result;
  for (const auto &[token, frequencies] : tokenFrequencies) {
      result.emplace_back(token, frequencies);
  }

  auto logarithmic_scale_frequency_compare = 
      [](const TokenInfo &lhs,
         const TokenInfo &rhs) -> bool {
    return lhs.frequencies_ > rhs.frequencies_ ||
           (lhs.frequencies_ == rhs.frequencies_ && lhs.str_ < rhs.str_);
  };

  std::sort(result.begin(), result.end(), logarithmic_scale_frequency_compare);

  if (count < result.size()) {
    result.resize(count);
  }

  return result;
}

#endif
