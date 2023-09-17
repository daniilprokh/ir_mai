#ifndef SPLIT_INTO_TOKENS_H_IR_MAI
#define SPLIT_INTO_TOKENS_H_IR_MAI

#include <boost/tokenizer.hpp>

#include <algorithm>
#include <functional>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

using StringFilter = std::function<bool (const std::string &)>;

inline  std::vector<std::string> SplitIntoTokens(
    std::string_view str,
    std::optional<StringFilter> filter = std::nullopt) {
  static boost::char_separator<char> sep{ " \n\t,.·?!\"'#-–&*()+=|/\\",
                                          "",
                                          boost::drop_empty_tokens };
  boost::tokenizer<boost::char_separator<char>> tokenizer(str, sep);
  
  std::vector<std::string> result;

  if (filter.has_value()) {
    std::copy_if(tokenizer.begin(), tokenizer.end(),
                 std::back_inserter(result),
                 filter.value());
  } else {
    std::copy(tokenizer.begin(), tokenizer.end(), std::back_inserter(result));
  }

  return result;
}

#endif
