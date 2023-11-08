#include <IRMAI/web_server/http_web_server.h>
#include <IRMAI/search_engine/ranked_searcher.h>

using namespace std;

int main(int argc, char* argv[]) {
  HTTPWebServer<RankedSearcher> app;
  return app.run(argc, argv);
}
