#include <IRMAI/inverted_index/inverted_index.h>
#include <IRMAI/inverted_index/inverted_index_initializer.h>
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
  filesystem::path index_path{argv[2]};

  Query query(argv[3]);

  auto index = InvertedIndexInitializer::LoadOrBuild(index_path, corpus_path);

  auto search_begin = chrono::steady_clock::now();
  auto documents = index.SearchQuery(query);
  auto search_end = chrono::steady_clock::now();
  auto search_diff = search_end - search_begin;
  cout << "Search time: " << 
    chrono::duration_cast<chrono::milliseconds>(search_diff).count() << " ms\n";
  
  cout << "Document count: " << documents.size() << '\n';
}
