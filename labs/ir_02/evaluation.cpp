#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

using namespace std;


double ComputePrecision(const vector<int>& result, size_t K) {
    return accumulate(result.begin(), next(result.begin(), K), 0.0) / K;
}

double ComputeDSG(const vector<int>& result, size_t K) {
  double res = 0.0;
  for (size_t k = 0; k < K; k += 1) {
    res += static_cast<double>(result[k]) / log2(k + 2);
  }
  return res;
}

double ComputeNDSG(const vector<int>& result, size_t K) {
  double idsg = 0.0;
  for (size_t k = 0; k < K; k += 1) {
    idsg += 1.0 / log2(k + 2);
  }
  return ComputeDSG(result, K) / idsg;
}

double ComputeERR(const vector<int>& result, size_t K) {
  vector<int> p;
  double res = 0.0;
  for (size_t k = 0; k < K; k += 1) {
    int p_k = result[k];
    for (int value : p) {
      p_k *= (1 - value);
    }
    p.push_back(result[k]);
    res += static_cast<double>(p_k) / (k + 1);
  }
  return res;
}

int main(int argc, char* argv[]) {
  if (argc == 1) {
    return -1;
  }

  ifstream input(argv[1]);

  std::vector<vector<int>> review;
  for (string line; getline(input, line);) {
    vector<int> scores;
    for (int idx = 0; idx < line.size(); idx += 2) {
      scores.push_back(line[idx] - 48);
    }
    review.push_back(move(scores));
  }
  input.close();

  vector<double> metric_sums(12);
  for (const auto& result : review) {
    metric_sums[0] += ComputePrecision(result, 1);
    metric_sums[1] += ComputePrecision(result, 3);
    metric_sums[2] += ComputePrecision(result, 5);
    metric_sums[3] += ComputeDSG(result, 1);
    metric_sums[4] += ComputeDSG(result, 3);
    metric_sums[5] += ComputeDSG(result, 5);
    metric_sums[6] += ComputeNDSG(result, 1);
    metric_sums[7] += ComputeNDSG(result, 3);
    metric_sums[8] += ComputeNDSG(result, 5);
    metric_sums[9] += ComputeERR(result, 1);
    metric_sums[10] += ComputeERR(result, 3);
    metric_sums[11] += ComputeERR(result, 5);
  }

  cout << review.size() << '\n';

  cout << "P@1: " << metric_sums[0] / review.size() << '\n';
  cout << "P@3: " << metric_sums[1] / review.size() << '\n';
  cout << "P@5: " << metric_sums[2] / review.size() << '\n';
  cout << "DSG@1: " << metric_sums[3] / review.size() << '\n';
  cout << "DSG@3: " << metric_sums[4] / review.size() << '\n';
  cout << "DSG@5: " << metric_sums[5] / review.size() << '\n';
  cout << "NDSG@1: " << metric_sums[6] / review.size() << '\n';
  cout << "NDSG@3: " << metric_sums[7] / review.size() << '\n';
  cout << "NDSG@5: " << metric_sums[8] / review.size() << '\n';
  cout << "ERR@1: " << metric_sums[9] / review.size() << '\n';
  cout << "ERR@3: " << metric_sums[10] / review.size() << '\n';
  cout << "ERR@5: " << metric_sums[11] / review.size() << '\n';
  return 0;
}