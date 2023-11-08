#include <IRMAI/search_engine/ranked_searcher.h>
#include <IRMAI/query/query.h>

#include <chrono>
#include <filesystem>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 4) {
      return -1;
  }

  filesystem::path corpus_path{argv[1]};
  filesystem::path ranked_searcher_path{argv[2]};

  Query query(argv[3]);

  auto searcher = RankedSearcherInitializer::LoadOrBuild(ranked_searcher_path,
                                                         corpus_path);

  auto search_begin = chrono::steady_clock::now();
  auto documents = searcher.SearchQuery(query);
  auto search_end = chrono::steady_clock::now();
  auto search_diff = search_end - search_begin;
  cout << "Search time: " << 
    chrono::duration_cast<chrono::milliseconds>(search_diff).count() << " ms\n";
  
  cout << "Document count: " << documents.size() << '\n';

  return 0;
}
