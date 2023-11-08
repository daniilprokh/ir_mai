#include <IRMAI/web_server/http_web_server.h>
#include <IRMAI/search_engine/inverted_index.h>

using namespace std;

int main(int argc, char* argv[]) {
  HTTPWebServer<InvertedIndex> app;
  return app.run(argc, argv);
}
