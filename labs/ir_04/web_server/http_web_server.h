#ifndef HTTTP_WEB_SERVER_H_IR_MAI
#define HTTTP_WEB_SERVER_H_IR_MAI

#include "http_request_handler.h"

#include <IRMAI/inverted_index/inverted_index_initializer.h>

#include <Poco/Net/HTTPServer.h>
#include <Poco/Util/Option.h>
#include <Poco/Util/OptionCallback.h>
#include <Poco/Util/OptionSet.h>
#include <Poco/Util/ServerApplication.h>

#include <optional>

#include <iostream>

using Poco::Net::HTTPServer;
using Poco::Util::Option;
using Poco::Util::OptionCallback;
using Poco::Util::OptionSet;
using Poco::Util::ServerApplication;

class HTTPWebServer : public ServerApplication {
  protected:

    void initialize(Application& self) override {
      ServerApplication::initialize(self);
    }

    void uninitialize() override {
      ServerApplication::uninitialize();
    }

    void Configure(const std::string &, const std::string &filePath) {
      loadConfiguration(filePath);

      path_storage_.corpus_path_ = config().getString("corpus_path");
      logger().information(path_storage_.corpus_path_);
      path_storage_.start_page_path_ = config().getString("start_page_path");
      path_storage_.result_page_path_ = config().getString("result_page_path");

      context_.index_ = InvertedIndexInitializer::LoadOrBuild(
          config().getString("index_data_path"),
          path_storage_.corpus_path_);
    }

    void defineOptions(OptionSet& options) override {
      ServerApplication::defineOptions(options);

      options.addOption(
          Option("config", "c")
          .required(true)
          .repeatable(false)
          .argument("file_path")
          .callback(OptionCallback<HTTPWebServer>(this, &HTTPWebServer::Configure)));
    }

    int main(const std::vector<std::string>& args) override {
      Poco::UInt16 port = static_cast<Poco::UInt16>(
          config().getUInt("port", 8080));
      HTTPServer srv(new RequestHandlerFactory(&context_, path_storage_),
                     port);
      srv.start();
      logger().information("HTTP Server started on port %hu.", port);

      waitForTerminationRequest();
      logger().information("Stopping HTTP Server...");
      srv.stop();

      return Application::EXIT_OK;
    }
  private:
    QueryContext context_;
    PathStorage path_storage_;
};

#endif
