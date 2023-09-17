#include "http_web_server.h"

using namespace std;

int main(int argc, char* argv[]) {
  HTTPWebServer app;
  return app.run(argc, argv);
}
