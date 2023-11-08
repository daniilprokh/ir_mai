#ifndef QUERY_HANDLER_H_IR_MAI
#define QUERY_HANDLER_H_IR_MAI

#include <IRMAI/query/query.h>

#include <IRMAI/corpus/document.h>
#include <IRMAI/get_text_from_file.h>
#include <IRMAI/split.h>

#include <Poco/JSON/Parser.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>

#include <cmath>
#include <filesystem>
#include <string>
#include <unordered_map>

using Poco::Net::HTMLForm;
using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Util::Application;

template <class Searcher>
struct QueryContext {
  Searcher searcher_;
  std::unordered_map<std::string, std::vector<Document>> cache_;
};

template <class Searcher>
class QueryHandler : public HTTPRequestHandler {
  public:
    QueryHandler(QueryContext<Searcher> *context,
                 const std::filesystem::path &corpusPath)
     : context_(context),
       corpus_path_(corpusPath)
    {}

    void handleRequest(HTTPServerRequest& request,
                       HTTPServerResponse& response) override {
      Application& app = Application::instance();
      app.logger().information(
          "Request from %s: %s %s",
          request.clientAddress().toString(),
          request.getMethod(),
          request.getURI()
      );

      Poco::Net::HTMLForm html_form(request);
      html_form.load(request);

      std::string query_str = html_form.get("query");

      std::string page_str = html_form.get("page");
      int page = std::stoi(page_str);

      app.logger().information("Query: %s, page: %d", query_str, page);

      auto &cache = context_->cache_;
      auto &searcher  = context_->searcher_;

      auto it = cache.find(query_str);
      if (it == cache.end()) {
        if (Query::IsBooleanQuery(query_str)) {
          Query query(query_str);
          it = cache.emplace(query_str, searcher.SearchQuery(query)).first;
        } else {
          auto terms = Split(query_str);
          it = cache.emplace(query_str, searcher.SearchDocuments(terms)).first;
        }
      }

      const auto &documents = it->second;

      int total_pages = std::ceil(
        static_cast<double>(documents.size()) / kSearchResultCount);

      Poco::JSON::Object json;
      json.set("page", page);
      json.set("total_pages", total_pages);
      json.set("total_documents", documents.size());

      Poco::JSON::Array arr(Poco::JSON_PRESERVE_KEY_ORDER);
      for (auto idx = page * kSearchResultCount,
                end_idx = std::min(idx + kSearchResultCount, documents.size());
           idx < end_idx;
           idx += 1) {
        Poco::JSON::Object obj;
        obj.set("id", documents[idx].Id());

        std::filesystem::path document_path =
           corpus_path_ / documents[idx].Filename() ;
        std::string text = GetTextFromFile(document_path);
        if (kTextLength < text.size()) {
          text.resize(kTextLength);
          text += "...";
        }

        obj.set("text", text);
        arr.add(obj);
      }
      json.set("data", arr);

      response.setChunkedTransferEncoding(true);
      response.setContentType("application/json");

      Poco::JSON::Stringifier::stringify(json, response.send());
    }
  private:
    const size_t kTextLength{250};
    const size_t kSearchResultCount{50};

    QueryContext<Searcher> *context_;
    std::filesystem::path corpus_path_;
};

#endif