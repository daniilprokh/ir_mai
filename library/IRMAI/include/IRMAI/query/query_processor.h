#ifndef QUERY_PROCESSOR_H_IR_MAI
#define QUERY_PROCESSOR_H_IR_MAI

#include <IRMAI/query/query.h>

#include <functional>
#include <stack>

class QueryProcessor {
  public:
    template <class Response>
    static Response Process(
        const Query &query,
        const std::function<Response (const std::string &)> &evaluateTerm,
        const std::function<void (Response &)> &evaluateNOT,
        const std::function<Response (
            Response &&,
            Response &&)> &evaluateAND,
        const std::function<Response (
            Response &&,
            Response &&)> &evaluateOR);
  private:
    template <class Response>
    static void evaluateBinaryOperation(
        std::stack<Response> &responses,
        const std::function<Response (
            Response &&,
            Response &&)> &evaluateOperation);
    template <class Response>
    static void evaluateUnaryOperation(
        std::stack<Response> &responses,
        const std::function<void (Response &)> &evaluateOperation);
};

template <class Response>
void QueryProcessor::evaluateBinaryOperation(
    std::stack<Response> &responses,
    const std::function<Response (
        Response &&,
        Response &&)> &evaluateOperation) {
  auto rhs = responses.top();
  responses.pop();
  auto lhs = responses.top();
  responses.pop();
  responses.push(evaluateOperation(std::move(lhs), std::move(rhs)));
}

template <class Response>
void QueryProcessor::evaluateUnaryOperation(
    std::stack<Response> &responses,
    const std::function<void (Response &)> &evaluateOperation) {
  auto &response = responses.top();
  evaluateOperation(response);
}

template <class Response>
Response QueryProcessor::Process(
    const Query &query,
    const std::function<Response (const std::string &)> &evaluateTerm,
    const std::function<void (Response &)> &evaluateNOT,
    const std::function<Response (
        Response &&,
        Response &&)> &evaluateAND,
    const std::function<Response (
        Response &&,
        Response &&)> &evaluateOR) {
  std::stack<Response> storage;

  for (const auto &element : query.elements_) {
    if (std::holds_alternative<std::string>(element)) {
      storage.push(evaluateTerm(std::get<std::string>(element)));
      continue;
    }

    switch (std::get<Query::OperatorType>(element))
    {
      case Query::OperatorType::kAND:
        evaluateBinaryOperation(storage, evaluateAND);
        break;
      case Query::OperatorType::kOR:
        evaluateBinaryOperation(storage, evaluateOR);
        break;
      case Query::OperatorType::kNOT:
        evaluateUnaryOperation(storage, evaluateNOT);
        break;
      default:
        break;
    }
  }

  return storage.top();
}

#endif
