#include <IRMAI/corpus/corpus_handler.h>
#include <IRMAI/corpus/document.h>
#include <IRMAI/get_text_from_file.h>
#include <IRMAI/split.h>
#include <IRMAI/to_lower.h>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

vector<size_t> ComputeLPSArray(const string& pattern) {
  vector<size_t> lps(pattern.size());
  size_t length = 0;
  for (size_t idx = 1; idx < pattern.size();) {
    if (pattern[idx] == pattern[length]) {
      length += 1;
      lps[idx] = length;
      idx += 1;
    } else if (length == 0) {
      lps[idx] = length;
      idx += 1;
    } else {
      length = lps[length - 1];
    }
  }
  return lps;
}

size_t KMPSearch(const string& text, const string& pattern) {
  vector<size_t> lps = ComputeLPSArray(pattern);

  size_t res = 0;

  size_t idx = 0;
  size_t jdx = 0;
  while (idx < text.size()) {
    if (text[idx] == pattern[jdx]) {
      idx += 1;
      jdx += 1;
    }

    if (jdx == pattern.size()) {
      jdx = lps[jdx - 1];
      res += 1;
    } else if (idx < text.size() && text[idx] != pattern[jdx]) {
      if (jdx == 0) {
        idx += 1;
      } else {
        jdx = lps[jdx - 1];
      }
    }
  }
  return res;
}

double ComputePrecision(const vector<size_t>& result, size_t K) {
    return accumulate(result.begin(), next(result.begin(), K), 0.0) / K;
}

double ComputeDSG(const vector<size_t>& result, size_t K) {
  double res = 0.0;
  for (size_t k = 0; k < K; k += 1) {
    res += static_cast<double>(result[k]) / log2(k + 2);
  }
  return res;
}

double ComputeNDSG(const vector<size_t>& result, size_t K) {
  double idsg = 0.0;
  for (size_t k = 0; k < K; k += 1) {
    idsg += 1.0 / log2(k + 2);
  }
  return ComputeDSG(result, K) / idsg;
}


int main(int argc, char* argv[]) {
  if (argc < 4) {
    return -1;
  }

  filesystem::path corpus_path{argv[1]};

  ifstream request_input(argv[2]);
  if (!request_input.is_open()) {
    return -1;
  }

  // vector<vector<string>> requests;
  // for (string line; getline(request_input, line);) {
  //  vector<string> request = Split(line);
  //  requests.push_back(move(request));
  // }

  vector<string> requests;
  for (string line; getline(request_input, line);) {
   requests.push_back(move(line));
  }

  request_input.close();

  vector<vector<pair<Document, size_t>>> results(requests.size());

  auto lambda = [&requests, &results]
      (const filesystem::directory_entry& corpusEntry) {
    string text = GetTextFromFile(corpusEntry.path());
    if (text.empty()) {
      return;
    }

    ToLower(text);

    for (size_t idx = 0; idx < requests.size(); idx += 1) {
      // size_t occurrence_sum = 0;
      // for (const string& word : requests[idx]) {
      //  occurrence_sum += KMPSearch(text, word);
      // }
      // results[idx].emplace_back(corpusEntry.path(), occurrence_sum);
      results[idx].emplace_back(corpusEntry.path(),
                                KMPSearch(text, requests[idx]));
    }
  };
  CorpusHandler::Pass(corpus_path, lambda);

  ofstream result_output(argv[3]);
  for (size_t idx = 0; idx < requests.size(); idx += 1) {
    auto& result = results[idx];
    sort(result.begin(), result.end(), [] (const auto &lhs, const auto &rhs) {
      return lhs.second > rhs.second;
    });
    result.resize(5);
    result_output << "Request: " << requests[idx] << '\n';
    //result_output << "Request: " << idx << '\n';
    for (const auto& [document, relevance] : result) {
      result_output << '\t' << document.Id() << " with rel " << relevance << '\n';
    }
  }
  result_output.close();

  // vector<double> metric_sums(9);
  // for (const auto& result : results) {
  //   metric_sums[0] += ComputePrecision(result, 1);
  //   metric_sums[1] += ComputePrecision(result, 3);
  //   metric_sums[2] += ComputePrecision(result, 5);
  //   metric_sums[3] += ComputeDSG(result, 1);
  //   metric_sums[4] += ComputeDSG(result, 3);
  //   metric_sums[5] += ComputeDSG(result, 5);
  //   metric_sums[6] += ComputeNDSG(result, 1);
  //   metric_sums[7] += ComputeNDSG(result, 3);
  //   metric_sums[8] += ComputeNDSG(result, 5);
  // }

  // cout << "P@1: " << metric_sums[0] / requests.size() << '\n';
  // cout << "P@3: " << metric_sums[1] / requests.size() << '\n';
  // cout << "P@5: " << metric_sums[2] / requests.size() << '\n';
  // cout << "DSG@1: " << metric_sums[3] / requests.size() << '\n';
  // cout << "DSG@3: " << metric_sums[4] / requests.size() << '\n';
  // cout << "DSG@5: " << metric_sums[5] / requests.size() << '\n';
  // cout << "NDSG@1: " << metric_sums[6] / requests.size() << '\n';
  // cout << "NDSG@3: " << metric_sums[7] / requests.size() << '\n';
  // cout << "NDSG@5: " << metric_sums[8] / requests.size() << '\n';

  return 0;
}
