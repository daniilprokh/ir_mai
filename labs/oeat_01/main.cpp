#include <IRMAI/corpus/corpus_handler.h>
#include <IRMAI/file_size_converter.h>
#include <IRMAI/get_text_from_file.h>
#include <IRMAI/split_into_tokens.h>
#include <IRMAI/to_lower.h>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_set>

using namespace std;

int main(int argc, char* argv[]) {
  auto program_begin = chrono::steady_clock::now();
  if (argc < 2) {
      return -1;
  }

  filesystem::path corpus_path{argv[1]};
  
  vector<string> texts;
  std::uintmax_t corpus_size = 0;
  auto get_texts_from_corpus = [&texts, &corpus_size] (
      const filesystem::directory_entry &corpusEntry) -> void {
    string text = GetTextFromFile(corpusEntry.path());
    if (text.empty()) {
      return;
    }

    corpus_size += corpusEntry.file_size();

    ToLower(text);
    texts.push_back(text);
  };
  CorpusHandler::Pass(corpus_path, get_texts_from_corpus);

  auto token_begin = chrono::steady_clock::now();
  vector<string> tokens;
  for (const string& text : texts) {
      auto text_tokens = SplitIntoTokens(text);
      move(text_tokens.begin(), text_tokens.end(), back_inserter(tokens));
  }
  auto token_end = chrono::steady_clock::now();
  auto token_diff = token_end - token_begin;
  auto token_time = chrono::duration_cast<chrono::seconds>(token_diff).count();
  cout << "Tokenization time: " << token_time << " s\n";

  double corpus_size_kb =
      FileSizeConverter::Convert(corpus_size,
                                 FileSizeConverter::BinaryPrefix::kMissing,
                                 FileSizeConverter::BinaryPrefix::kK);
  cout << "Processing speed: " << corpus_size_kb / token_time << " KB/s\n";

  unordered_set<std::string> unique_tokens;
  size_t token_length_sum = 0;
  for (const string& token : tokens) {
    bool inserted = unique_tokens.insert(token).second;
    if (inserted) {
      token_length_sum += token.size();
    }
  }

  cout << "Number of tokens: " << unique_tokens.size() << '\n';
  cout << "Average token length: " <<
     static_cast<double>(token_length_sum) / unique_tokens.size() << '\n';

  auto program_end = chrono::steady_clock::now();
  auto program_diff = program_end - program_begin;
  cout << "Total working time: " <<
    chrono::duration_cast<chrono::seconds>(program_diff).count() << " s\n";

  return 0;
}
