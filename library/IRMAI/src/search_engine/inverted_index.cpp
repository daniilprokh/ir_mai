#include <IRMAI/search_engine/inverted_index.h>

#include <IRMAI/query/query_processor.h>

#include <algorithm>
#include <fstream>

std::vector<Document> InvertedIndex::FindDocuments(const std::string &term) {
  auto it = data_.find(term);
  return it != data_.end() 
      ? std::vector<Document>(it->second)
      : std::vector<Document>();
}

void InvertedIndex::OnInitialize(const std::vector<DocumentTokens> &corpusTokens) {
  for (const auto& [document, tokens] : corpusTokens) {
    corpus_documents_.push_back(document);

    for (const std::string &token : tokens) {
      std::vector<Document>& token_documents = data_[token];
      if (token_documents.empty() || document != token_documents.back()) {
        token_documents.push_back(document);
      }
    }
  }

  std::sort(corpus_documents_.begin(), corpus_documents_.end());
}

std::vector<Document> InvertedIndex::OnSearchDocuments(
    const std::vector<std::string> &terms) {
  if (terms.size() == 0) {
    return std::vector<Document>();
  }
    
  std::vector<std::vector<Document>> vecs_to_intersect;
  for (const std::string& term : terms) {
    std::vector<Document> documents = FindDocuments(term);
    std::sort(documents.begin(), documents.end());
    vecs_to_intersect.push_back(std::move(documents));
  }

  std::vector<Document> result(vecs_to_intersect.front());
  for (size_t idx = 1; idx < vecs_to_intersect.size(); idx += 1) {
    std::vector<Document> new_result;
    std::set_intersection(
      result.begin(), result.end(),
      vecs_to_intersect[idx].begin(), vecs_to_intersect[idx].end(),
      std::back_inserter(new_result)
    );

    if (new_result.empty()) {
      return new_result;
    }
    
    result = std::move(new_result);
  }

  return result;
}

std::vector<Document> InvertedIndex::OnSearchQuery(const Query &query) {
  auto evalution_term = 
      [this](const std::string &term) -> std::vector<Document> {
    std::vector<Document> documents = this->FindDocuments(term);
    std::sort(documents.begin(), documents.end());
    return documents;
  };

  auto evalution_operation_not = 
      [&corpus_documents = corpus_documents_](
          std::vector<Document> &documents) -> void {
    std::vector<Document> other_documents;
    std::set_difference(corpus_documents.begin(), corpus_documents.end(),
                        documents.begin(), documents.end(),
                        std::inserter(other_documents, other_documents.begin()));
    documents = std::move(other_documents);
  };

  auto evalution_operation_and =
      [](std::vector<Document> &&lhs,
         std::vector<Document> &&rhs) -> std::vector<Document> {
    std::vector<Document> documents;
    std::set_intersection(lhs.begin(), lhs.end(),
                          rhs.begin(), rhs.end(),
                          std::back_inserter(documents));
    return documents;
  };

  auto evalution_operation_or =
      [](std::vector<Document> &&lhs,
         std::vector<Document> &&rhs) -> std::vector<Document> {
    std::vector<Document> documents;
    std::set_union(lhs.begin(), lhs.end(),
                   rhs.begin(), rhs.end(),
                   std::back_inserter(documents));
    return documents;
  };

  return QueryProcessor::Process<std::vector<Document>>(
      query,
      evalution_term,
      evalution_operation_not,
      evalution_operation_and,
      evalution_operation_or);
}
