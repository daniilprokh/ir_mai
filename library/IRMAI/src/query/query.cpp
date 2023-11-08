#include <IRMAI/query/query.h>

#include <IRMAI/to_lower.h>

#include <stack>
#include <string>

Query::Query(std::string_view queryStr) {
  ParseStringInInfixFormat(std::move(queryStr));
  OrderFromInfixToPostfix();
}

void Query::ParseStringInInfixFormat(std::string_view &&queryStr) {
  size_t pos = queryStr.find_first_not_of(' ');
  if (pos == queryStr.npos) {
    return;
  }

  std::vector<QueryElement> elements;
  while (pos != queryStr.npos) {
    switch (queryStr[pos])
    {
      case '!':
      case '(':
      case ')':
        elements.emplace_back(static_cast<OperatorType>(queryStr[pos]));
        pos = queryStr.find_first_not_of(' ', pos + 1);
        continue;
    }

    size_t end_pos = queryStr.find(' ', pos + 1);
    std::string str(queryStr.substr(pos, end_pos - pos));
    if (str == "&&" || str == "||")  {
      elements.emplace_back(static_cast<OperatorType>(str.front()));
    } else {
      ToLower(str);
      elements.emplace_back(str);
    }

    pos = queryStr.find_first_not_of(' ', end_pos);
  }

  auto isOperatorANDRequired = 
      [] (const QueryElement &lhs, const QueryElement &rhs) -> bool {
    return 
      (std::holds_alternative<std::string>(lhs) ||
        std::get<OperatorType>(lhs) == OperatorType::kClosingBracket) &&
      (std::holds_alternative<std::string>(rhs) ||
        std::get<OperatorType>(rhs) == OperatorType::kOpeningBracket ||
        std::get<OperatorType>(rhs) == OperatorType::kNOT);
  };
  
  elements_.push_back(std::move(elements.front()));
  for (size_t idx = 1; idx < elements.size(); idx += 1) {
    if (isOperatorANDRequired(elements_.back(), elements[idx])) {
      elements_.emplace_back(OperatorType::kAND);
    }

    elements_.push_back(std::move(elements[idx]));
  }
}

void Query::OrderFromInfixToPostfix() {
  std::vector<QueryElement> postfix_order;
  std::stack<OperatorType> operations;
  for (const auto &element : elements_) {
    if (std::holds_alternative<std::string>(element)) {
      postfix_order.push_back(std::move(element));
      continue;
    }

    OperatorType operation = std::get<OperatorType>(element);
    if (operations.empty()) {
      operations.push(operation);
      continue;
    }

    switch (operation)
    {
      case OperatorType::kClosingBracket:
        while (operations.top() != OperatorType::kOpeningBracket)
        {
          postfix_order.emplace_back(operations.top());
          operations.pop();
        }
        operations.pop();
        break;
      case OperatorType::kAND:
      case OperatorType::kOR:
        if (operations.top() == OperatorType::kOpeningBracket) {
          operations.push(operation);
          break;
        }

        do {
          postfix_order.emplace_back(operations.top());
          operations.pop();
        } while (
            !operations.empty() && 
            operations.top() == OperatorType::kOpeningBracket);

        operations.push(operation);

        break;
      case OperatorType::kNOT:
      case OperatorType::kOpeningBracket:
        operations.push(operation);
        break;
    }
  }

  while (!operations.empty()) {
    postfix_order.emplace_back(operations.top());
    operations.pop();
  }

  elements_ = std::move(postfix_order);
}

bool Query::IsBooleanQuery(std::string_view queryStr) {
  for (char c : queryStr) {
    switch (c)
    {
      case '&':
      case '!':
      case '|':
      case '(':
      case ')':
        return true;
      default:
        break;
    }
  }

  return false;
}