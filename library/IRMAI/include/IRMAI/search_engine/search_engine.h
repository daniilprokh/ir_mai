#ifndef SEARCH_ENGINE_H_IR_MAI
#define SEARCH_ENGINE_H_IR_MAI

#include <IRMAI/search_engine/search_engine_initializer.h>

#include <IRMAI/corpus/document.h>
#include <IRMAI/corpus/document_tokens.h>
#include <IRMAI/query/query.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/vector.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

template <class Derived>
class SearchEngine {
  public:
    SearchEngine() = default;

    std::vector<Document> SearchDocuments(
        const std::vector<std::string> &terms) {
      return static_cast<Derived *>(this)->OnSearchDocuments(terms);
    }
    std::vector<Document> SearchQuery(const Query &query) {
      return static_cast<Derived *>(this)->OnSearchQuery(query);
    }
  protected:
    friend class SearchEngineInitializer<Derived>;

    std::vector<Document> corpus_documents_;
    std::unordered_map<std::string, std::vector<Document>> data_;

    void Initialize(const std::vector<DocumentTokens> &corpusTokens);
    void Deserialize(const std::filesystem::path &path);
    void Serialize(const std::filesystem::path &path);
};

template <class Derived>
void SearchEngine<Derived>::Initialize(
    const std::vector<DocumentTokens> &corpusTokens) {
  static_cast<Derived *>(this)->OnInitialize(corpusTokens);
}

template <class Derived>
void SearchEngine<Derived>::Deserialize(const std::filesystem::path &path) {
  std::ifstream f(path, std::ios::binary);
  if (!f.is_open()) {
    return;
  }

  boost::archive::binary_iarchive ia(f);
  ia >> data_;
  ia >> corpus_documents_;

  static_cast<Derived *>(this)->OnDeserialize(ia);

  f.close();
}

template <class Derived>
void SearchEngine<Derived>::Serialize(const std::filesystem::path &path) {
  std::ofstream f(path, std::ios::binary);
  if (!f.is_open()) {
    return;
  }

  boost::archive::binary_oarchive oa(f);
  oa << data_;
  oa << corpus_documents_;

  static_cast<Derived *>(this)->OnSerialize(oa);

  f.close();
}

#endif