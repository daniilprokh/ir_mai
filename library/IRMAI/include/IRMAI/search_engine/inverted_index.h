#ifndef INVERTED_INDEX_H_IR_MAI
#define INVERTED_INDEX_H_IR_MAI

#include <IRMAI/search_engine/search_engine.h>

#include <IRMAI/corpus/document.h>
#include <IRMAI/corpus/document_tokens.h>
#include <IRMAI/query/query.h>

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class InvertedIndex : public SearchEngine<InvertedIndex> {
public:
  InvertedIndex() = default;
private:
  friend SearchEngine<InvertedIndex>;

  std::vector<Document> FindDocuments(const std::string &term);

  void OnDeserialize(boost::archive::binary_iarchive &) {}
  void OnInitialize(const std::vector<DocumentTokens> &corpusTokens);
  void OnSerialize(boost::archive::binary_oarchive &) {}

  std::vector<Document> OnSearchDocuments(
      const std::vector<std::string> &terms);
  std::vector<Document> OnSearchQuery(const Query &query);
};

using InvertedIndexInitializer = SearchEngineInitializer<InvertedIndex>;

#endif //! INVERTED_INDEX_H_IR_MAI
