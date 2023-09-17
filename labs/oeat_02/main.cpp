#include <IRMAI/corpus/corpus_handler.h>
#include <IRMAI/get_text_from_file.h>
#include <IRMAI/top_frequencies.h>
#include <IRMAI/split_into_tokens.h>
#include <IRMAI/to_lower.h>

#include <algorithm>
#include <cctype>
#include <charconv>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>

using namespace std;

int main(int argc, char* argv[]) {
  if (argc < 4) {
      return -1;
  }

  filesystem::path corpus_path{ argv[1] };
  int top_count = atoi(argv[2]);
  filesystem::path output_path{ argv[3] };

  std::unordered_map<std::string, unsigned long long> token_frequencies;
  auto calculate_token_frequencies = [&token_frequencies] (
      const filesystem::directory_entry &corpusEntry) -> void {
    string text = GetTextFromFile(corpusEntry.path());
    if (text.empty()) {
        return;
    }

    ToLower(text);

    auto filter = [](const std::string &str) -> bool {
      return std::all_of(str.begin(), str.end(),
                         [](unsigned char c) { return std::isalpha(c); });
    };

    for (const string &token : SplitIntoTokens(text, filter)) {
      token_frequencies[token] += 1;
    }
  };
  CorpusHandler::Pass(corpus_path, calculate_token_frequencies);

  auto top_frequencies =
    TopFrequencies(token_frequencies, top_count);
  
  ofstream output(output_path); 
  if (!output.is_open()) {
    cout << output_path << '\n';
    return -1;
  }

  for (const auto& [frequencies, str] : top_frequencies) {
    output << frequencies << ' ' << str << '\n';
  }

  output.close();

  return 0;
}
