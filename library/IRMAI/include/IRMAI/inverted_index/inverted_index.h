#ifndef INVERTED_INDEX_H_IR_MAI
#define INVERTED_INDEX_H_IR_MAI

#include <IRMAI/corpus/document.h>
#include <IRMAI/corpus/document_tokens.h>
#include <IRMAI/query/query.h>

#include <filesystem>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

class InvertedIndex {
public:
  InvertedIndex() = default;

  std::vector<Document> SearchDocuments(
    const std::vector<std::string> &terms);

  std::vector<Document> SearchQuery(const Query &query);

private:

  friend class InvertedIndexInitializer;

  InvertedIndex(
    const std::vector<DocumentTokens> &corpusTokens);

  void Serialize(const std::filesystem::path& path);
  void Deserialize(const std::filesystem::path& path);

  std::vector<Document> FindDocuments(const std::string &term);

  std::vector<Document> corpus_documents_;

  std::unordered_map<std::string, std::vector<Document>> data_;
};

#endif //! INVERTED_INDEX_H_IR_MAI
