#include <IRMAI/inverted_index/inverted_index.h>
#include <IRMAI/inverted_index/inverted_index_initializer.h>
#include <IRMAI/split.h>
#include <IRMAI/to_lower.h>

#include <filesystem>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 4) {
      return -1;
  }

  filesystem::path corpus_path{argv[1]};
  filesystem::path index_path{argv[2]};

  vector<string> terms = Split(argv[3], ',');
  for (auto &term : terms) {
    ToLower(term);
  }

  auto index = InvertedIndexInitializer::LoadOrBuild(index_path, corpus_path);

  for (const Document &document : index.SearchDocuments(terms)) {
    cout << document.Id() << '\n';
  }

  return 0;
}
