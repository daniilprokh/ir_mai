#ifndef RANKED_SEARCHER_H_IR_MAI
#define RANKED_SEARCHER_H_IR_MAI

#include <IRMAI/search_engine/search_engine.h>

#include <IRMAI/corpus/document.h>
#include <IRMAI/corpus/document_tokens.h>
#include <IRMAI/query/query.h>

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class RankedSearcher : public SearchEngine<RankedSearcher> {
public:
  RankedSearcher() = default;
private:
  friend SearchEngine<RankedSearcher>;

  //std::vector<Document> FindDocuments(const std::string &term);

  using DocumentRelevance = std::unordered_map<Document, double>;

  void RankDocuments(std::vector<Document> &documents,
                     const std::unordered_set<std::string> &tokens);

  void OnDeserialize(boost::archive::binary_iarchive &iarchive);
  void OnInitialize(const std::vector<DocumentTokens> &corpusTokens);
  void OnSerialize(boost::archive::binary_oarchive &oarchive);

  std::vector<Document> OnSearchDocuments(
      const std::vector<std::string> &terms);
  std::vector<Document> OnSearchQuery(const Query &query);

  std::unordered_map<std::string, DocumentRelevance> relevance_;
};

using RankedSearcherInitializer = SearchEngineInitializer<RankedSearcher>;

#endif