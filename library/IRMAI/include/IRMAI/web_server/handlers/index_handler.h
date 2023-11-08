#ifndef INDEX_HANDLER_H_IR_MAI
#define INDEX_HANDLER_H_IR_MAI

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include <Poco/Util/ServerApplication.h>

#include <filesystem>

using Poco::Net::HTTPRequestHandler;
using Poco::Net::HTTPServerRequest;
using Poco::Net::HTTPServerResponse;
using Poco::Util::Application;

class IndexHandler: public HTTPRequestHandler {
  public:
    IndexHandler(const std::filesystem::path &indexPath)
      : index_path_(indexPath)
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

      response.setChunkedTransferEncoding(true);
      response.sendFile(index_path_.string(), "text/html");
    }
  private:
    std::filesystem::path index_path_;
};

#endif
