#ifndef QUERY_H_IR_MAI
#define QUERY_H_IR_MAI

#include <string_view>
#include <variant>
#include <vector>

class Query {
  public:
    Query() = delete;

    explicit Query(std::string_view queryStr);
  private:

    friend class QueryProcessor;

    enum class OperatorType : char {
      kAND = '&',
      kOR = '|',
      kNOT = '!',
      kOpeningBracket = '(',
      kClosingBracket = ')'
    };

    using QueryElement = std::variant<std::string, OperatorType>;

    void ParseStringInInfixFormat(std::string_view &&queryStr);

    void OrderFromInfixToPostfix();


    std::vector<QueryElement> elements_;
};

#endif
