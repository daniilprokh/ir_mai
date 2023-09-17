#include <IRMAI/inverted_index/inverted_index_initializer.h>

#include <IRMAI/corpus/corpus_handler.h>
#include <IRMAI/corpus/document.h>
#include <IRMAI/file_size_converter.h>
#include <IRMAI/get_text_from_file.h>
#include <IRMAI/split_into_tokens.h>
#include <IRMAI/to_lower.h>

#include <iostream>
#include <chrono>

InvertedIndex InvertedIndexInitializer::Build(
    const std::filesystem::path& corpusPath) {
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

  auto index_start = std::chrono::steady_clock::now();
  InvertedIndex index(corpus_tokens);
  auto index_end = std::chrono::steady_clock::now();
  auto index_diff = index_end - index_start;
  auto index_time = 
      std::chrono::duration_cast<std::chrono::seconds>(index_diff).count();
  std::cout << "Indexing time: " << index_time << " s\n";

  double corpus_size_kb =
      FileSizeConverter::Convert(corpus_size,
                                 FileSizeConverter::BinaryPrefix::kMissing,
                                 FileSizeConverter::BinaryPrefix::kK);
  std::cout << "Indexing speed: " << corpus_size_kb / index_time << " KB/s\n";

  return index;
}

std::optional<InvertedIndex> InvertedIndexInitializer::Load(
    const std::filesystem::path &indexPath) {
  if (!std::filesystem::exists(indexPath)) {
    return std::optional<InvertedIndex>();
  }

  InvertedIndex index;
  index.Deserialize(indexPath);
  return std::optional<InvertedIndex>(index);
}
InvertedIndex InvertedIndexInitializer::LoadOrBuild(
    const std::filesystem::path &indexPath,
    const std::filesystem::path &corpusPath) {
  InvertedIndex index;
  if (std::filesystem::exists(indexPath)) {
    index.Deserialize(indexPath);
  } else {
    index = Build(corpusPath);
    index.Serialize(indexPath);
  }
  return index;
}