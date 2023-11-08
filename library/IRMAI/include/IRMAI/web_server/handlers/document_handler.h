#ifndef DOCUMENT_HANDLER_H_IR_MAI
#define DOCUMENT_HANDLER_H_IR_MAI

#include <IRMAI/split.h>

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>

#include <filesystem>
#include <vector>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Util::Application;

class DocumentHandler: public HTTPRequestHandler {
  public:
    DocumentHandler(const std::filesystem::path &corpusPath)
      : corpus_path_(corpusPath)
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

      std::vector<std::string> res = Split(request.getURI(), '/');
      //boost::split(res, request.getURI(), boost::is_any_of("/"));

      std::filesystem::path file_path = res.back() + ".txt";
      std::filesystem::path absolute_file_path = corpus_path_ / file_path;
      if (!std::filesystem::exists(absolute_file_path)) {
        return;
      }

      response.setChunkedTransferEncoding(true);
      response.sendFile(absolute_file_path.string(), "text/plain");
    }
  private:
    std::filesystem::path corpus_path_;
};

#endif
