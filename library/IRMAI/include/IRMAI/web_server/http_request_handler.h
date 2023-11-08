#ifndef HTTP_REQUEST_HANDLER_H_IR_MAI
#define HTTP_REQUEST_HANDLER_H_IR_MAI

#include "handlers/document_handler.h"
#include "handlers/index_handler.h"
#include "handlers/query_handler.h"

#include <IRMAI/contains_substr.h>

#include <Poco/Net/HTTPRequestHandlerFactory.h>

#include <filesystem>
#include <unordered_map>

using Poco::Net::HTTPRequestHandlerFactory;

struct PathStorage {
  std::filesystem::path corpus_path_;
  std::filesystem::path start_page_path_;
  std::filesystem::path result_page_path_;
};

template <class Searcher>
class RequestHandlerFactory : public HTTPRequestHandlerFactory {
  public:
    RequestHandlerFactory(QueryContext<Searcher>* context,
                          const PathStorage& pathStorage)
      : context_(context),
        path_storage_(pathStorage)
    {}

    HTTPRequestHandler* createRequestHandler(
        const HTTPServerRequest& r) override {
      const std::string& uri = r.getURI();

      if (ContainsSubstr(uri, "/query")) {
        return new QueryHandler<Searcher>(context_, path_storage_.corpus_path_);
      }

      if (ContainsSubstr(uri, "/doc")) {
        return new DocumentHandler(path_storage_.corpus_path_);
      }

      if (ContainsSubstr(uri, "/result")) {
        return new IndexHandler(path_storage_.result_page_path_);
      }

      return new IndexHandler(path_storage_.start_page_path_);
    } 
  private:
    QueryContext<Searcher>* context_;
    PathStorage path_storage_;
};

#endif
