#ifndef SEARCH_ENGINE_INITIALIZER_H_IR_MAI
#define SEARCH_ENGINE_INITIALIZER_H_IR_MAI

#include <IRMAI/corpus/corpus_handler.h>
#include <IRMAI/corpus/document.h>
#include <IRMAI/corpus/document_tokens.h>
#include <IRMAI/file_size_converter.h>
#include <IRMAI/get_text_from_file.h>
#include <IRMAI/split_into_tokens.h>
#include <IRMAI/to_lower.h>

template <class Searcher>
class SearchEngineInitializer{
  public:
    static Searcher Build(const std::filesystem::path &corpusPath);
    static std::optional<Searcher> Load(
        const std::filesystem::path &searchEngineDataPath);
    static Searcher LoadOrBuild(
        const std::filesystem::path &searchEngineDataPath,
        const std::filesystem::path &corpusPath);
};

template <class Searcher>
Searcher SearchEngineInitializer<Searcher>::Build(
    const std::filesystem::path &corpusPath) {
  std::vector<std::filesystem::directory_entry> documents;

  auto document_collector = [&documents] 
      (const std::filesystem::directory_entry &corpusEntry) -> void {
    documents.push_back(corpusEntry);
  };
  CorpusHandler::Pass(corpusPath, document_collector);

  std::vector<DocumentTokens> corpus_tokens;

  std::uintmax_t corpus_size = 0;
  for (const auto &document : documents) {
    std::string text = GetTextFromFile(document.path());
    if (text.empty()) {
      continue;
    }

    ToLower(text);
    corpus_tokens.emplace_back(document.path(), SplitIntoTokens(text));

    corpus_size += document.file_size();
  }
  
  Searcher searcher;
  auto start = std::chrono::steady_clock::now();
  searcher.Initialize(corpus_tokens);
  auto end = std::chrono::steady_clock::now();
  auto diff = end - start;
  auto time = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
  std::cout << "Initialization time: " << time << " s\n";

  double corpus_size_kb =
      FileSizeConverter::Convert(corpus_size,
                                 FileSizeConverter::BinaryPrefix::kMissing,
                                 FileSizeConverter::BinaryPrefix::kK);
  std::cout << "Initialization speed: " << corpus_size_kb / time << " KB/s\n";

  return searcher;
}

template <class Searcher>
std::optional<Searcher> SearchEngineInitializer<Searcher>::Load(
    const std::filesystem::path &searchEngineDataPath) {
   if (!std::filesystem::exists(searchEngineDataPath)) {
    return std::optional<Searcher>();
  }

  Searcher searcher;
  searcher.Deserialize(searchEngineDataPath);
  return std::optional<Searcher>(searcher);
}

template <class Searcher>
Searcher SearchEngineInitializer<Searcher>::LoadOrBuild(
    const std::filesystem::path &searchEngineDataPath,
    const std::filesystem::path &corpusPath) {
  Searcher searcher;
  if (std::filesystem::exists(searchEngineDataPath)) {
    searcher.Deserialize(searchEngineDataPath);
  } else {
    searcher = Build(corpusPath);
    searcher.Serialize(searchEngineDataPath);
  }
  return searcher;
}

#endif