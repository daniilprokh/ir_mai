#ifndef RANKED_SEARCH_H_IR_MAI
#define RANKED_SEARCH_H_IR_MAI

#include <IRMAI/search_engine/ranked_searcher.h>

#include <IRMAI/query/query_processor.h>

#include <cmath>

void RankedSearcher::RankDocuments(
    std::vector<Document> &documents,
    const std::unordered_set<std::string> &tokens) {
  std::vector<std::pair<Document, double>> ranked_documents;
  for (Document& document : documents) {
    double rank_measure = 0.0;
    for (const std::string &token : tokens) {
      const auto &token_relevance = relevance_[token];
      auto it = token_relevance.find(document);
      if (it != token_relevance.end()) {
        rank_measure += relevance_[token][document];
      }
    }

    ranked_documents.emplace_back(std::move(document), rank_measure);
  }

  std::sort(ranked_documents.begin(), ranked_documents.end(),
            [] (auto &lhs, auto &rhs) {
              return lhs.second > rhs.second;
            });

  for (size_t idx = 0; idx < documents.size(); idx += 1) {
    documents[idx] = std::move(ranked_documents[idx].first);
  }
}

void RankedSearcher::OnDeserialize(boost::archive::binary_iarchive &iarchive)
{
  iarchive >> relevance_;
}

void RankedSearcher::OnInitialize(const std::vector<DocumentTokens> &corpusTokens) {
  std::unordered_map<std::string, DocumentRelevance> term_frequency;

  for (const auto& [document, tokens] : corpusTokens) {
    corpus_documents_.push_back(document);

    std::unordered_map<std::string, size_t> token_count;

    for (const std::string &token : tokens) {

      std::vector<Document>& token_documents = data_[token];
      if (token_documents.empty() || document != token_documents.back()) {
        token_count[token] = 1;
        token_documents.push_back(document);
      } else {
        token_count[token] += 1;
      }
    }

    for (auto &[token, count] : token_count) {
      term_frequency[token][document] =
         static_cast<double>(count) / tokens.size();
    }
  }

  for (auto &[token, documents] : data_) {
    double inverse_document_frequency =
        std::log(documents.size() / corpus_documents_.size()) ;
    
    for (const Document &document : documents) {
      relevance_[token][document] =
         term_frequency[token][document] * inverse_document_frequency;
    }
  }

  std::sort(corpus_documents_.begin(), corpus_documents_.end());
}

void RankedSearcher::OnSerialize(boost::archive::binary_oarchive &oarchive) {
  oarchive << relevance_;
}

std::vector<Document> RankedSearcher::OnSearchDocuments(
    const std::vector<std::string> &terms) {
  if (terms.size() == 0) {
    return std::vector<Document>();
  }

  std::unordered_set<std::string> tokens;
  std::vector<std::vector<Document>> vecs_to_intersect;
  for (const std::string& term : terms) {
    auto it_documents = data_.find(term);
    if (it_documents == data_.end()) {
      continue;
    }

    bool is_new_token = tokens.insert(term).second;
    if (is_new_token) {
      std::vector<Document> documents = it_documents->second;
      std::sort(documents.begin(), documents.end());
      vecs_to_intersect.push_back(std::move(documents));
    }
  }

  if (vecs_to_intersect.empty()) {
    return std::vector<Document>();
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

  RankDocuments(result, tokens);

  return result;
}

std::vector<Document> RankedSearcher::OnSearchQuery(const Query &query) {
  std::unordered_set<std::string> tokens;
  auto evalution_term = 
      [this, &tokens](const std::string &term) -> std::vector<Document> {
    tokens.insert(term);
    auto it = data_.find(term);
    std::vector<Document> documents = 
        it != data_.end() ? it->second 
                          : std::vector<Document>();
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

  auto result = QueryProcessor::Process<std::vector<Document>>(
      query,
      evalution_term,
      evalution_operation_not,
      evalution_operation_and,
      evalution_operation_or);
  
  RankDocuments(result, tokens);

  return result;
}

#endif